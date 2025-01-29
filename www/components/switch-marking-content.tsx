import { useLanguage } from "@/contexts/language-context";
import { Box, Flex, Center, Stack, IconButton, Button, VStack, Badge } from "@chakra-ui/react";
import { SegmentedControl } from "./ui/segmented-control";
import { Line } from 'react-chartjs-2';
import { Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend, ChartData, ChartOptions } from 'chart.js';
import { useEffect, useRef, useState } from "react";
import { MenuContent, MenuItem, MenuRoot, MenuTrigger } from "./ui/menu";
import { LuTrash, LuPlus, LuMenu, LuStar } from "react-icons/lu";
import { openForm } from "./dialog-form";
import { PROFILE_NAME_MAX_LENGTH } from "@/types/gamepad-config";
import { openConfirm } from "./dialog-confirm";
import { useGamepadConfig } from "@/contexts/gamepad-config-context";
import useUnsavedChangesWarning from "@/hooks/use-unsaved-changes-warning";

// 注册Chart.js组件
ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

const options: ChartOptions<"line"> = {
    responsive: true,
    plugins: {
        legend: {
            position: 'top' as const,
            display: false,
        },
        title: {
            display: false,
            text: 'Chart.js Line Chart',
        },
    },
    animation: {
        duration: 500,
        easing: 'easeInOutCubic',
    }
};



export function SwitchMarkingContent() {
    const { t } = useLanguage();
    const [_isDirty, setIsDirty] = useUnsavedChangesWarning(t.SETTINGS_SWITCH_MARKING_UNSAVED_CHANGES_WARNING_TITLE, t.SETTINGS_SWITCH_MARKING_UNSAVED_CHANGES_WARNING_MESSAGE);

    const [mappingData, setMappingData] = useState<ChartData<"line">>({
        labels: [],
        datasets: []
    });

    const { mappingNameList, fetchMappingNameList } = useGamepadConfig();
    const { defaultMappingName, fetchDefaultMapping } = useGamepadConfig();
    const { markingStatus, fetchMarkingStatus } = useGamepadConfig();
    const { startMarking, stopMarking, stepMarking } = useGamepadConfig();
    const { createMapping, deleteMapping, updateDefaultMapping } = useGamepadConfig();
    const [ activeMappingName, setActiveMappingName ] = useState<string>("");
    const { activeMapping, fetchActiveMapping } = useGamepadConfig();
    const [ markingStatusToastMessage, setMarkingStatusToastMessage ] = useState<string>("");
    const nextActiveMappingNameRef = useRef<string>(activeMappingName);
    const timerRef = useRef<NodeJS.Timeout | null>(null);

    useEffect(() => {
        fetchMappingNameList();
        fetchDefaultMapping();
        fetchMarkingStatus();

        return () => {
            if(timerRef.current) {
                clearInterval(timerRef.current);
                timerRef.current = null;
            }
            if(markingStatus?.is_marking) {
                stopMarking();
            }
        }
    }, []);

    useEffect(() => {

        if(nextActiveMappingNameRef.current && nextActiveMappingNameRef.current !== "" && nextActiveMappingNameRef.current !== activeMappingName) {
            setActiveMappingName(nextActiveMappingNameRef.current);
            nextActiveMappingNameRef.current = "";
            return;
        }

        if(activeMappingName && activeMappingName !== "" && mappingNameList.includes(activeMappingName)) {
            return;
        }

        if(defaultMappingName && defaultMappingName !== "") {
            setActiveMappingName(defaultMappingName);
        } else if(mappingNameList.length > 0) {
            setActiveMappingName(mappingNameList[0]);
        } else {
            setActiveMappingName("");
        }
    }, [mappingNameList]);

    useEffect(() => {
        // 如果标记中，则设置为脏状态，跳转页面时弹出确认框
        if(markingStatus?.is_marking) {
            setIsDirty(true);
        } else {
            setIsDirty(false);
        }

        // 如果标记中，但标记的不是当前映射，则停止标记
        if(markingStatus?.is_marking && markingStatus?.mapping_name !== activeMapping?.name) {
            stopMarking();
        }

        const activeMappingIsMarking = (markingStatus?.mapping_name === activeMapping?.name);

        if(activeMappingIsMarking) {
            const myData = {
                labels: Array.from({ length: markingStatus?.length ?? 0 }, (_, i) => ((i + 1) * (markingStatus?.step ?? 0)).toFixed(2)),
                datasets: [
                    {
                        label: markingStatus.mapping_name,
                        cubicInterpolationMode: 'monotone' as const,
                        tension: .4,
                        fill: true,
                        backgroundColor: 'rgba(75,192,192,0.2)',
                        borderColor: 'rgba(75,192,192,1)',
                        data: markingStatus.values,
                    },
                ],
            };  

            setMappingData(myData);

        } else {

            const myData = {
                labels: Array.from({ length: activeMapping?.length ?? 0 }, (_, i) => ((i + 1) * (activeMapping?.step ?? 0)).toFixed(2)),
                datasets: [
                    {
                        label: activeMapping?.name ?? "",
                        cubicInterpolationMode: 'monotone' as const,
                        tension: .4,
                        fill: true,
                        backgroundColor: 'rgba(75,192,192,0.2)',
                        borderColor: 'rgba(75,192,192,1)',
                        data: activeMapping?.originalValues ?? [],
                    },
                ],
            };

            setMappingData(myData);

        }
    }, [activeMapping, markingStatus]);

    useEffect(() => {
        if(activeMappingName && activeMappingName !== "" && mappingNameList.includes(activeMappingName)) {
            fetchActiveMapping(activeMappingName);
        }
    }, [activeMappingName]);

    // 更新标记状态提示信息
    useEffect(() => {
        if(!markingStatus) {
            setMarkingStatusToastMessage("");
            return;
        }

        if(markingStatus.is_marking && !timerRef.current) {
            timerRef.current = setInterval(() => {
                fetchMarkingStatus();
            }, 2000);
        } else if(!markingStatus.is_marking && timerRef.current) {
            clearInterval(timerRef.current);
            timerRef.current = null;
        }

        // 如果标记未开始，则弹出提示
        if(!markingStatus.is_marking && !markingStatus.is_completed && !markingStatus.is_sampling) {
            setMarkingStatusToastMessage(t.SETTINGS_SWITCH_MARKING_START_DIALOG_MESSAGE);
        // 如果标记完成，则弹出提示
        } else if(!markingStatus.is_marking && markingStatus.is_completed) {
            setMarkingStatusToastMessage(t.SETTINGS_SWITCH_MARKING_COMPLETED_DIALOG_MESSAGE);
        // 如果标记开始，则弹出提示
        } else if(markingStatus.is_marking && !markingStatus.is_completed && !markingStatus.is_sampling) {
            // 如果步进即将完成，则弹出保存提示
            if(markingStatus.index >= markingStatus.length) {
                setMarkingStatusToastMessage(t.SETTINGS_SWITCH_MARKING_SAVE_DIALOG_MESSAGE);
            // 如果步进未完成，则弹出步进提示
            } else {
                const step = markingStatus.index + 1;   
                const distance = (step * (markingStatus.step ?? 0)).toFixed(2);
                setMarkingStatusToastMessage(t.SETTINGS_SWITCH_MARKING_SAMPLING_START_DIALOG_MESSAGE.replace("<step>", step.toString()).replace("<distance>", distance));
            }
        // 如果采样中，则弹出提示
        } else if(markingStatus.is_sampling) {
            setMarkingStatusToastMessage(t.SETTINGS_SWITCH_MARKING_SAMPLING_DIALOG_MESSAGE.replace("<step>", (markingStatus.index + 1).toString()).replace("<distance>", (markingStatus.index * (markingStatus.step ?? 0)).toFixed(2)));
        } else {
            setMarkingStatusToastMessage("");
        }
    }, [markingStatus]);

    const createMappingClick = async () => {
        const result = await openForm({
            title: t.SETTINGS_SWITCH_MARKING_TITLE,
            fields: [{
                name: "name",
                label: t.SETTINGS_SWITCH_MARKING_NAME_LABEL,
                placeholder: t.SETTINGS_SWITCH_MARKING_NAME_PLACEHOLDER,
                type: "text",
                defaultValue: "",
                validate: (value: string) => {
                    const [isValid, errorMessage] = validateSwitchMarkingName(value);
                    if (!isValid) {
                        return errorMessage;
                    }
                    return undefined;
                }
            }, {
                name: "length",
                label: t.SETTINGS_SWITCH_MARKING_LENGTH_LABEL,
                placeholder: t.SETTINGS_SWITCH_MARKING_LENGTH_PLACEHOLDER,
                type: "number",
                defaultValue: "1",
                min: 1,
                max: 50,
                step: 1,
                validate: (value: string) => {
                    const num = parseInt(value);
                    const [isValid, errorMessage] = validateSwitchMarkingLength(num);
                    if (!isValid) {
                        return errorMessage;
                    }
                    return undefined;
                }
            }, {
                name: "step",
                label: t.SETTINGS_SWITCH_MARKING_STEP_LABEL,
                placeholder: t.SETTINGS_SWITCH_MARKING_STEP_PLACEHOLDER,
                type: "number",
                defaultValue: "0.01",
                min: 0.01,
                max: 10,
                step: 0.01,
                validate: (value: string) => {
                    const num = parseFloat(value);
                    const [isValid, errorMessage] = validateSwitchMarkingStep(num);
                    if (!isValid) {
                        return errorMessage;
                    }
                    return undefined;
                }
            }]
        });

        if (result) {
            await createMapping(result.name, parseInt(result.length), parseFloat(result.step));
            nextActiveMappingNameRef.current = result.name;
            await fetchMappingNameList();
        }
    }

    const validateSwitchMarkingName = (name: string): [boolean, string] => {

        if (/[!@#$%^&*()_+\[\]{}|;:'",.<>?/\\]/.test(name)) {
            return [false, t.SETTINGS_SWITCH_MARKING_VALIDATION_SPECIAL_CHARS];
        }

        if (name.length > PROFILE_NAME_MAX_LENGTH || name.length < 1) {
            return [false, t.SETTINGS_SWITCH_MARKING_VALIDATION_LENGTH.replace("{0}", name.length.toString())];
        }

        if (mappingNameList.find(p => p === name)) {
            return [false, t.SETTINGS_SWITCH_MARKING_VALIDATION_SAME_NAME];
        }

        return [true, ""];
    }

    const validateSwitchMarkingLength = (length: number): [boolean, string] => {
        if (length < 1 || length > 50) {
            return [false, t.SETTINGS_SWITCH_MARKING_VALIDATION_LENGTH_RANGE.replace("{0}", length.toString())];
        }
        return [true, ""];
    }

    const validateSwitchMarkingStep = (step: number): [boolean, string] => {
        if (step < .1 || step > 10) {
            return [false, t.SETTINGS_SWITCH_MARKING_VALIDATION_STEP_RANGE.replace("{0}", step.toString())];
        }
        return [true, ""];
    }

    const deleteMappingClick = async () => {
        const confirmed = await openConfirm({
            title: t.SETTINGS_SWITCH_MARKING_DELETE_DIALOG_TITLE,
            message: t.SETTINGS_SWITCH_MARKING_DELETE_CONFIRM_MESSAGE
        });

        if (confirmed) {
            await deleteMapping(activeMapping?.name ?? '');
            await fetchMappingNameList();
        }
    }

    const setDefaultMappingClick = async () => {
        if(activeMapping) {
            await updateDefaultMapping(activeMapping.name);
        }
    }

    const activeMappingChange = (name: string) => {
        setActiveMappingName(name);
    }

    const menuItems = [
        {
            value: "create",
            label: "Add New",
            icon: <LuPlus />,
            onClick: createMappingClick
        },
        {
            value: "delete",
            label: "Delete",
            icon: <LuTrash />,
            onClick: deleteMappingClick
        },
        {
            value: "set_default",
            label: "Set Default",
            icon: <LuStar />,
            onClick: setDefaultMappingClick,
            disabled: activeMappingName === defaultMappingName,
        }
    ];

    return (
        <>
            <Flex direction={"column"} height={"100%"} width={"1700px"} >
                <VStack width={"100%"} >
                    <Center width={"100%"} >
                        <Stack direction="row" gap={2} alignItems="center">
                            <SegmentedControl value={activeMappingName} items={mappingNameList} onValueChange={(detail) => activeMappingChange(detail.value)} />
                            <MenuRoot>
                                <MenuTrigger asChild>
                                    <IconButton
                                        aria-label="Menu"
                                        variant="ghost"
                                        size="sm"
                                    >
                                        <LuMenu />
                                    </IconButton>
                                </MenuTrigger>
                                <MenuContent>
                                    {menuItems.map((item) => (
                                        <MenuItem key={item.value} value={item.value} onClick={item.disabled? undefined : item.onClick} disabled={item.disabled}>
                                            {item.icon} {item.label}
                                        </MenuItem>
                                    ))}
                                </MenuContent>
                            </MenuRoot>
                            <Button 
                                colorPalette={ markingStatus?.is_marking ? "red" : "green" } 
                                size="xs" variant={ !markingStatus?.is_marking ? "solid" : "outline" } 
                                onClick={() => {
                                if(markingStatus?.is_marking) {
                                    stopMarking();
                                } else {
                                    startMarking(activeMappingName);
                                }
                            }}>
                                { markingStatus?.is_marking ? "Stop Marking" : "Start Marking" }
                            </Button>
                            <Button 
                                colorPalette={"green"} size="xs" 
                                variant={ markingStatus?.is_marking ? "solid" : "outline" } 
                                disabled={!markingStatus?.is_marking || markingStatus?.is_sampling} 
                                onClick={() => {
                                stepMarking();
                            }}>
                                { "Step" }
                            </Button>
                        </Stack>
                    </Center>
                    <Center width={"100%"} height={"2em"} paddingTop={"1em"} >
                        <Badge colorPalette={"green"} variant={"outline"} size="sm" >{ markingStatusToastMessage }</Badge>
                    </Center>
                </VStack>
                <Box width={"100%"} flexGrow={1} padding={"18px 0"} >
                    <Line data={mappingData} options={options} />
                </Box>
            </Flex>
        </>
    );
}
