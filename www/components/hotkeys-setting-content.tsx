"use client";

import {
    Flex,
    Center,
    Stack,
    Fieldset,
    Button,
    Text,
} from "@chakra-ui/react";
import { useEffect, useState, useMemo } from "react";
import {
    HotkeyAction,
    DEFAULT_NUM_HOTKEYS_MAX,
    Hotkey,
} from "@/types/gamepad-config";
import Hitbox from "@/components/hitbox";
import HotkeysField from "./hotkeys-field";
import { toaster } from "@/components/ui/toaster";
import { useGamepadConfig } from "@/contexts/gamepad-config-context";
import useUnsavedChangesWarning from "@/hooks/use-unsaved-changes-warning";
import { openDialog as openRebootDialog } from "@/components/dialog-cannot-close";
import { openConfirm as openRebootConfirmDialog } from "@/components/dialog-confirm";
import { useLanguage } from "@/contexts/language-context";

export function HotkeysSettingContent() {
    const { t } = useLanguage();
    const { hotkeysConfig, updateHotkeysConfig, fetchHotkeysConfig, rebootSystem } = useGamepadConfig();
    const [_isDirty, setIsDirty] = useUnsavedChangesWarning();

    const [hotkeys, setHotkeys] = useState<Hotkey[]>([]);
    const [activeHotkeyIndex, setActiveHotkeyIndex] = useState<number>(0);

    useEffect(() => {
        // 从 gamepadConfig 加载 hotkeys 配置
        setHotkeys(Array.from({ length: DEFAULT_NUM_HOTKEYS_MAX }, (_, i) => {
            return hotkeysConfig?.[i] ?? { key: -1, action: HotkeyAction.None, isLocked: false };
        }));
        setIsDirty?.(false);
    }, [hotkeysConfig]);

    useMemo(() => {
        if (activeHotkeyIndex >= 0 && hotkeys[activeHotkeyIndex]?.isLocked === true) {
            const index = hotkeys.findIndex(h => h.isLocked === undefined || h.isLocked === false);
            if (index >= 0) {
                setActiveHotkeyIndex(index);
            }
        }
    }, [hotkeys]);

    const saveHotkeysConfigHandler = async () => {
        if (!hotkeysConfig) return;
        await updateHotkeysConfig(hotkeys);
    };

    const updateHotkey = (index: number, hotkey: Hotkey) => {
        if (index < 0 || index >= DEFAULT_NUM_HOTKEYS_MAX) return;

        const keys = hotkeys.map(h => h.key);
        const keyIndex = keys.indexOf(hotkey.key);
        // 如果 hotkey 的 key 已经在 hotkeys 中，并且不是当前正在编辑的 hotkey，则不更新
        if (keyIndex >= 0 && keyIndex !== index && hotkey.key >= 0) {
            toaster.create({
                title: t.ERROR_KEY_ALREADY_BINDED_TITLE,
                description: t.ERROR_KEY_ALREADY_BINDED_DESC,
                type: "error",
            });
            return;
        }

        const newHotkeys = hotkeys.slice();
        newHotkeys[index] = hotkey;
        setHotkeys(newHotkeys);
        setIsDirty?.(true); // 更新 dirty 状态
    };

    const handleHitboxClick = (id: number) => {
        if (id >= 0 && id < 19) {
            updateHotkey(activeHotkeyIndex, { ...hotkeys[activeHotkeyIndex], key: id });
        }
    };

    return (
        <Flex direction="row" width="1700px" padding="18px">
            <Center width="100%" flex={1}>
                <Hitbox
                    interactiveIds={[...Array(19).fill(0).map((_, i) => i)]} // 0-18 共19个按键可以交互，并设置为hotkey
                    onClick={handleHitboxClick}
                />
            </Center>
            <Center width="700px">
                <Fieldset.Root>
                    <Stack direction="column" gap={4}>
                        <Fieldset.Legend fontSize="2rem" color="green.600">
                            {t.SETTINGS_HOTKEYS_TITLE}
                        </Fieldset.Legend>
                        <Fieldset.HelperText fontSize="smaller" opacity={0.75} >
                            <Text whiteSpace="pre-wrap" >{t.SETTINGS_HOTKEYS_HELPER_TEXT}</Text>
                        </Fieldset.HelperText>
                        <Fieldset.Content pt="30px" >
                            <Stack gap={4}>
                                {Array.from({ length: DEFAULT_NUM_HOTKEYS_MAX }, (_, i) => (
                                    <HotkeysField
                                        key={i}
                                        index={i}
                                        value={hotkeys[i] ?? { key: -1, action: HotkeyAction.None }}
                                        onValueChange={(changeDetail) => {
                                            updateHotkey(i, changeDetail);
                                        }}
                                        isActive={i === activeHotkeyIndex}
                                        onFieldClick={(index) => setActiveHotkeyIndex(index)}
                                        disabled={hotkeys[i]?.isLocked ?? false}
                                    />
                                ))}

                                <Stack direction="row" gap={4} justifyContent="flex-start" padding="32px 0px">
                                    <Button
                                        colorPalette="teal"
                                        variant="surface"
                                        size="lg"
                                        width="140px"
                                        onClick={fetchHotkeysConfig}
                                    >
                                        {t.BUTTON_RESET}
                                    </Button>
                                    <Button
                                        colorPalette="green"
                                        size="lg"
                                        width="140px"
                                        onClick={saveHotkeysConfigHandler}
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
                                                await saveHotkeysConfigHandler();
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