"use client";

import {
    Flex,
    Center,
    Stack,
    Fieldset,
    SimpleGrid,
    Button,
    HStack,
    RadioCardLabel,
    Text,
} from "@chakra-ui/react";
import KeymappingFieldset from "@/components/keymapping-fieldset";
import { useEffect, useState } from "react";
import { Switch } from "@/components/ui/switch";
import {
    RadioCardItem,
    RadioCardRoot,
} from "@/components/ui/radio-card"
import {
    GameProfile,
    GameSocdMode,
    GameSocdModeList,
    GameSocdModeLabelMap,
    Platform,
    PlatformList,
    PlatformLabelMap,
    GameControllerButton,
} from "@/types/gamepad-config";
import { SegmentedControl } from "@/components/ui/segmented-control";
import Hitbox from "@/components/hitbox";
import { LuInfo } from "react-icons/lu";
import { ToggleTip } from "@/components/ui/toggle-tip"
import { useGamepadConfig } from "@/contexts/gamepad-config-context";
import useUnsavedChangesWarning from "@/hooks/use-unsaved-changes-warning";
import { openDialog as openRebootDialog } from "@/components/dialog-cannot-close";
import { openConfirm as openRebootConfirmDialog } from "@/components/dialog-confirm";
import { useLanguage } from "@/contexts/language-context";

export function KeysSettingContent() {

    const { defaultProfile, updateProfileDetails, resetProfileDetails, rebootSystem } = useGamepadConfig();
    const [_isDirty, setIsDirty] = useUnsavedChangesWarning();  
    const { t } = useLanguage();

    const [inputMode, setInputMode] = useState<Platform>(Platform.XINPUT);
    const [socdMode, setSocdMode] = useState<GameSocdMode>(GameSocdMode.SOCD_MODE_UP_PRIORITY);
    const [invertXAxis, setInvertXAxis] = useState<boolean>(false);
    const [invertYAxis, setInvertYAxis] = useState<boolean>(false);
    const [fourWayMode, setFourWayMode] = useState<boolean>(false);
    const [keyMapping, setKeyMapping] = useState<{ [key in GameControllerButton]?: number[] }>({});
    const [autoSwitch, setAutoSwitch] = useState<boolean>(true);
    const [inputKey, setInputKey] = useState<number>(-1);

    useEffect(() => {
        setInputMode(defaultProfile.keysConfig?.inputMode ?? Platform.XINPUT);
        setSocdMode(defaultProfile.keysConfig?.socdMode ?? GameSocdMode.SOCD_MODE_UP_PRIORITY);
        setInvertXAxis(defaultProfile.keysConfig?.invertXAxis ?? false);
        setInvertYAxis(defaultProfile.keysConfig?.invertYAxis ?? false);
        setFourWayMode(defaultProfile.keysConfig?.fourWayMode ?? false);
        setKeyMapping(defaultProfile.keysConfig?.keyMapping ?? {});
        setIsDirty?.(false); // reset the unsaved changes warning 
    }, [defaultProfile]);


    /**
     * set key mapping
     * @param key - game controller button
     * @param hitboxButtons - hitbox buttons
     */
    const setHitboxButtons = (key: string, hitboxButtons: number[]) => {
        setKeyMapping({
            ...keyMapping,
            [key as GameControllerButton]: hitboxButtons,
        });
    }

    const hitboxButtonClick = (keyId: number) => {
        setInputKey(keyId);
    }

    const saveProfileDetailHandler = () => {

        const newProfile: GameProfile = {
            id: defaultProfile.id,
            keysConfig: {
                invertXAxis: invertXAxis,
                invertYAxis: invertYAxis,
                fourWayMode: fourWayMode,
                socdMode: socdMode,
                inputMode: inputMode,
                keyMapping: keyMapping,
            },
        }

        updateProfileDetails(defaultProfile.id, newProfile);
    }

    return (
        <>
            <Flex direction="row" width={"1700px"} padding={"18px"} >
                <Center flex={1}  >
                    <Hitbox 
                        onClick={hitboxButtonClick}
                        interactiveIds={[ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 ]} 
                    />
                </Center>
                <Center width={"700px"}  >
                    <Fieldset.Root>
                        <Stack direction={"column"} gap={4} >
                            <Fieldset.Legend fontSize={"2rem"} color={"green.600"} >
                                {t.SETTINGS_KEYS_TITLE}
                            </Fieldset.Legend>
                            <Fieldset.HelperText fontSize={"smaller"} opacity={0.75}   >
                                <Text whiteSpace="pre-wrap" >{t.SETTINGS_KEYS_HELPER_TEXT}</Text>
                            </Fieldset.HelperText>
                            <Fieldset.Content position={"relative"} paddingTop={"30px"}  >

                                {/* Input Mode Choice */}
                                <RadioCardRoot
                                    colorPalette={"green"}
                                    size={"sm"}
                                    variant={"surface"}
                                    value={inputMode?.toString() ?? Platform.XINPUT.toString()}
                                    onValueChange={(detail) => {
                                        setInputMode(detail.value as Platform);
                                        setIsDirty?.(true);
                                    }}
                                >
                                    <RadioCardLabel>{t.SETTINGS_KEYS_INPUT_MODE_TITLE}</RadioCardLabel>
                                    <SimpleGrid gap={1} columns={5} >
                                        {PlatformList.map((platform, index) => (
                                            // <Tooltip key={index} content={t.SETTINGS_KEYS_PLATFORM_MODE_TOOLTIP} >
                                                <RadioCardItem
                                                    fontSize={"xs"}
                                                    indicator={false}
                                                    key={index}
                                                    value={platform.toString()}
                                                    label={PlatformLabelMap.get(platform as Platform)?.label ?? ""}
                                                />
                                            // </Tooltip>
                                        ))}
                                    </SimpleGrid>
                                </RadioCardRoot>

                                {/* Key Mapping */}
                                <Stack direction={"column"} gap={4} >
                                    <Fieldset.Legend fontSize={"sm"} >{t.SETTINGS_KEYS_MAPPING_TITLE}</Fieldset.Legend>
                                    <HStack gap={1} >
                                        <SegmentedControl
                                            size={"xs"}
                                            defaultValue={autoSwitch ? t.SETTINGS_KEY_MAPPING_AUTO_SWITCH_LABEL : t.SETTINGS_KEY_MAPPING_MANUAL_SWITCH_LABEL}
                                            items={[t.SETTINGS_KEY_MAPPING_AUTO_SWITCH_LABEL, t.SETTINGS_KEY_MAPPING_MANUAL_SWITCH_LABEL]}
                                            onValueChange={(detail) => setAutoSwitch(detail.value === t.SETTINGS_KEY_MAPPING_AUTO_SWITCH_LABEL)}
                                        />
                                        <ToggleTip content={t.TOOLTIP_AUTO_SWITCH}  >
                                            <Button size="xs" variant="ghost">
                                                <LuInfo />
                                            </Button>
                                        </ToggleTip>
                                    </HStack>
                                    <KeymappingFieldset
                                        autoSwitch={autoSwitch}
                                        inputKey={inputKey}
                                        inputMode={inputMode}
                                        keyMapping={keyMapping}
                                        changeKeyMappingHandler={(key, hitboxButtons) => {
                                            setHitboxButtons(key, hitboxButtons);
                                            setIsDirty?.(true);
                                        }}
                                    />
                                </Stack>

                                


                                {/* SOCD Mode Choice */}
                                <RadioCardRoot
                                    colorPalette={"green"}
                                    size={"sm"}
                                    variant={"surface"}
                                    value={socdMode?.toString() ?? GameSocdMode.SOCD_MODE_UP_PRIORITY.toString()}
                                    onValueChange={(detail) => {
                                        setSocdMode(detail.value as GameSocdMode);
                                        setIsDirty?.(true);
                                    }}
                                >
                                    <RadioCardLabel>{t.SETTINGS_KEYS_SOCD_MODE_TITLE}</RadioCardLabel>
                                    <SimpleGrid gap={1} columns={5} >
                                        {GameSocdModeList.map((socdMode, index) => (
                                            // <Tooltip key={index} content={t.SETTINGS_KEYS_SOCD_MODE_TOOLTIP} >
                                                <RadioCardItem
                                                    fontSize={"xs"}
                                                    indicator={false}
                                                    key={index}
                                                    value={socdMode.toString()}
                                                    label={GameSocdModeLabelMap.get(socdMode as GameSocdMode)?.label ?? ""}
                                                />
                                            // </Tooltip>
                                        ))}
                                    </SimpleGrid>
                                </RadioCardRoot>

                                {/* Invert Axis Choice & Invert Y Axis Choice & FourWay Mode Choice */}
                                <HStack gap={5} mt={4} >
                                    <Switch 
                                        colorPalette={"green"} 
                                        checked={invertXAxis} 
                                        onChange={() => {
                                            setInvertXAxis(!invertXAxis);
                                            setIsDirty?.(true);
                                        }}
                                    >
                                        {t.SETTINGS_KEYS_INVERT_X_AXIS}
                                    </Switch>
                                    <Switch 
                                        colorPalette={"green"} 
                                        checked={invertYAxis} 
                                        onChange={() => {
                                            setInvertYAxis(!invertYAxis);
                                            setIsDirty?.(true);
                                        }}
                                    >
                                        {t.SETTINGS_KEYS_INVERT_Y_AXIS}
                                    </Switch>
                                    {/* <Switch colorPalette={"green"} checked={fourWayMode} onChange={() => setFourWayMode(!fourWayMode)} >
                                        {t.SETTINGS_KEYS_FOURWAY_MODE}
                                    </Switch>  
                                    <ToggleTip content={t.SETTINGS_KEYS_FOURWAY_MODE_TOOLTIP} >
                                        <Button size="xs" variant="ghost">
                                            <LuInfo />
                                        </Button>
                                    </ToggleTip> */}
                                </HStack>

                            </Fieldset.Content>
                            <Stack direction={"row"} gap={4} justifyContent={"flex-start"} padding={"32px 0px"} >
                                <Button colorPalette={"teal"} variant={"surface"} size={"lg"} width={"140px"} onClick={resetProfileDetails}>
                                    {t.BUTTON_RESET}
                                </Button>
                                <Button colorPalette={"green"} size={"lg"} width={"140px"} onClick={saveProfileDetailHandler}>
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
                    </Fieldset.Root>
                </Center>
            </Flex>
        </>
    )
}
