"use client";

import {
    Flex,
    Center,
    Stack,
    Fieldset,
    Button,
    Text,
} from "@chakra-ui/react";
import { Slider } from "@/components/ui/slider";
import { Switch } from "@/components/ui/switch";
import { useEffect, useState } from "react";
import { RapidTriggerConfig } from "@/types/gamepad-config";
import Hitbox from "@/components/hitbox";
import { useGamepadConfig } from "@/contexts/gamepad-config-context";
import useUnsavedChangesWarning from "@/hooks/use-unsaved-changes-warning";
import { openDialog as openRebootDialog } from "@/components/dialog-cannot-close";
import { openConfirm as openRebootConfirmDialog } from "@/components/dialog-confirm";
import { useLanguage } from "@/contexts/language-context";

interface TriggerConfig {
    topDeadzone: number;
    bottomDeadzone: number;
    pressAccuracy: number;
    releaseAccuracy: number;
}

const defaultTriggerConfig: TriggerConfig = {
    topDeadzone: 0,
    bottomDeadzone: 0,
    pressAccuracy: 0,
    releaseAccuracy: 0
};

export function RapidTriggerContent() {
    const { defaultProfile, updateProfileDetails, resetProfileDetails, rebootSystem } = useGamepadConfig();
    const [_isDirty, setIsDirty] = useUnsavedChangesWarning();
    const { t } = useLanguage();

    const [selectedButton, setSelectedButton] = useState<number | null>(0); // 当前选中的按钮
    const [triggerConfigs, setTriggerConfigs] = useState<RapidTriggerConfig[]>([]); // 按钮配置
    const [isAllBtnsConfiguring, setIsAllBtnsConfiguring] = useState(false); // 是否同时配置所有按钮
    const [allBtnsConfig, setAllBtnsConfig] = useState<RapidTriggerConfig>({ ...defaultTriggerConfig });

    // 所有按钮的键值
    const allKeys = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

    /**
     * 加载触发配置
     */
    useEffect(() => {
        // Load trigger configs from gamepadConfig when it changes
        const triggerConfigs = { ...defaultProfile.triggerConfigs };
        setIsAllBtnsConfiguring(triggerConfigs.isAllBtnsConfiguring ?? false);
        setTriggerConfigs(allKeys.map(key => triggerConfigs.triggerConfigs?.[key] ?? defaultTriggerConfig));
        setAllBtnsConfig(triggerConfigs.triggerConfigs?.[0] ?? defaultTriggerConfig);
        setIsDirty?.(false);
    }, [defaultProfile]);

    /**
     * 获取当前配置
     */
    const getCurrentConfig = () => {
        if (selectedButton === null) return defaultTriggerConfig;
        return triggerConfigs[selectedButton] ?? defaultTriggerConfig;
    };

    /**
     * 更新当前配置
     */
    const updateConfig = (key: keyof TriggerConfig, value: number) => {
        if (selectedButton === null) return;

        triggerConfigs[selectedButton] = {
            ...getCurrentConfig(),
            [key]: value
        };
        setTriggerConfigs(triggerConfigs);
    };

    /**
     * 更新所有按钮配置
     */
    const updateAllBtnsConfig = (key: keyof RapidTriggerConfig, value: number) => {
        setAllBtnsConfig({
            ...allBtnsConfig,
            [key]: value
        });
    };

    /**
     * 切换所有按钮配置
     */
    const switchAllBtnsConfiging = (n: boolean) => {
        if (n === true) {
            setAllBtnsConfig({
                ...allBtnsConfig,
                ...getCurrentConfig()
            });
        }
        setIsAllBtnsConfiguring(n);
    }

    /**
     * 保存配置
     */
    const saveProfileDetailHandler = async () => {
        const profileId = defaultProfile.id;
        if (isAllBtnsConfiguring) {
            const newTriggerConfigs: RapidTriggerConfig[] = [];
            allKeys.forEach((key, index) => {
                newTriggerConfigs[index] = allBtnsConfig;
            });

            await updateProfileDetails(profileId, {
                id: profileId,
                triggerConfigs: {
                    isAllBtnsConfiguring: isAllBtnsConfiguring,
                    triggerConfigs: newTriggerConfigs
                }
            });
        } else {
            await updateProfileDetails(profileId, {
                id: profileId,
                triggerConfigs: {
                    isAllBtnsConfiguring: isAllBtnsConfiguring,
                    triggerConfigs: triggerConfigs
                }
            });
        }
    };

    /**
     * 点击按钮
     */
    const handleButtonClick = (id: number) => {
        console.log("handleButtonClick: ", id);
        if (!isAllBtnsConfiguring && selectedButton !== id && id >= 0) {
            setSelectedButton(id);
        }
    };

    return (
        <Flex direction="row" width="1700px" padding={"18px"} >
            <Center width="100%" flex={1} >
                <Hitbox
                    onClick={(id) => handleButtonClick(id)}
                    highlightIds={!isAllBtnsConfiguring ? [selectedButton ?? -1] : allKeys}
                    interactiveIds={allKeys}
                />
            </Center>
            <Center width="700px" >
                <Fieldset.Root width="100%">
                    <Stack direction="column" gap={4}>
                        <Fieldset.Legend fontSize="2rem" color="green.600" >
                            {t.SETTINGS_RAPID_TRIGGER_TITLE}
                        </Fieldset.Legend>
                        <Fieldset.HelperText fontSize="smaller" opacity={0.75} >
                            <Text whiteSpace="pre-wrap" >{t.SETTINGS_RAPID_TRIGGER_HELPER_TEXT}</Text>
                        </Fieldset.HelperText>
                        <Fieldset.Content pt="30px" >
                            <Stack gap={6}>

                                <Switch
                                    colorPalette={"green"}
                                    checked={isAllBtnsConfiguring}
                                    onChange={() => {
                                        switchAllBtnsConfiging(!isAllBtnsConfiguring);
                                        setIsDirty?.(true);
                                    }}
                                >
                                    <Text fontSize={"sm"} opacity={0.75} >{t.SETTINGS_RAPID_TRIGGER_CONFIGURE_ALL}</Text>
                                </Switch>

                                <Text opacity={!isAllBtnsConfiguring ? "0.75" : "0.25"} fontSize={"sm"} >
                                    {(selectedButton !== null && !isAllBtnsConfiguring) ?
                                        t.SETTINGS_RAPID_TRIGGER_ONFIGURING_BUTTON
                                        : t.SETTINGS_RAPID_TRIGGER_SELECT_A_BUTTON_TO_CONFIGURE
                                    }
                                    {(selectedButton !== null && !isAllBtnsConfiguring) && (
                                        <Text as="span" fontWeight="bold">
                                            KEY-{(selectedButton ?? 0) + 1}
                                        </Text>
                                    )}
                                </Text>

                                {/* Sliders */}
                                {[
                                    { key: 'topDeadzone', label: t.SETTINGS_RAPID_TRIGGER_TOP_DEADZONE_LABEL },
                                    { key: 'bottomDeadzone', label: t.SETTINGS_RAPID_TRIGGER_BOTTOM_DEADZONE_LABEL },
                                    { key: 'pressAccuracy', label: t.SETTINGS_RAPID_TRIGGER_PRESS_ACCURACY_LABEL },
                                    { key: 'releaseAccuracy', label: t.SETTINGS_RAPID_TRIGGER_RELEASE_ACCURACY_LABEL },
                                ].map(({ key, label }) => (
                                    <Stack key={key} gap={6} >
                                        <Slider
                                            label={label}
                                            value={[isAllBtnsConfiguring ? allBtnsConfig[key as keyof RapidTriggerConfig] : getCurrentConfig()[key as keyof TriggerConfig]]}
                                            colorPalette={"green"}
                                            min={0}
                                            max={1}
                                            step={0.1}
                                            onValueChange={(details) => {
                                                if (isAllBtnsConfiguring) {
                                                    updateAllBtnsConfig(key as keyof RapidTriggerConfig, details.value[0]);
                                                } else {
                                                    updateConfig(key as keyof TriggerConfig, details.value[0]);
                                                }
                                                setIsDirty?.(true);
                                            }}
                                            disabled={selectedButton === null && !isAllBtnsConfiguring}
                                            width={"400px"}
                                            marks={[
                                                { value: 0, label: '0' },
                                                { value: 0.2, label: '0.2' },
                                                { value: 0.4, label: '0.4' },
                                                { value: 0.6, label: '0.6' },
                                                { value: 0.8, label: '0.8' },
                                                { value: 1, label: '1' },
                                            ]}
                                        />
                                        <Text fontSize="sm" opacity={0.75} >
                                            Value: {isAllBtnsConfiguring ? allBtnsConfig[key as keyof RapidTriggerConfig] : getCurrentConfig()[key as keyof TriggerConfig]}
                                        </Text>
                                    </Stack>
                                ))}

                                {/* Buttons */}
                                <Stack direction="row" gap={4} justifyContent="flex-start" padding="32px 0px">
                                    <Button
                                        colorPalette="teal"
                                        variant="surface"
                                        size="lg"
                                        width="140px"
                                        onClick={resetProfileDetails}
                                    >
                                        {t.BUTTON_RESET}
                                    </Button>
                                    <Button
                                        colorPalette="green"
                                        size="lg"
                                        width="140px"
                                        onClick={saveProfileDetailHandler}
                                    >
                                        {t.BUTTON_SAVE}
                                    </Button>
                                    <Button
                                        colorPalette="blue"
                                        variant="surface"
                                        size={"lg"}
                                        width={"180px"}
                                        onClick={async () => {
                                            const confirmed = await openRebootConfirmDialog({
                                                title: t.DIALOG_REBOOT_CONFIRM_TITLE,
                                                message: t.DIALOG_REBOOT_CONFIRM_MESSAGE,
                                            });
                                            if (confirmed) {
                                                await saveProfileDetailHandler();
                                                await rebootSystem();
                                                openRebootDialog({
                                                    title: t.DIALOG_REBOOT_SUCCESS_TITLE,
                                                    status: "success",
                                                    message: t.DIALOG_REBOOT_SUCCESS_MESSAGE,
                                                });
                                            }
                                        }}
                                    >
                                        {t.BUTTON_REBOOT_WITH_SAVING}
                                    </Button>
                                </Stack>
                            </Stack>
                        </Fieldset.Content>
                    </Stack>
                </Fieldset.Root>
            </Center>
        </Flex>
    );
} 