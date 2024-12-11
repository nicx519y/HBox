export const NUM_PROFILES_MAX = 8;
// LEDS animation cycle in milliseconds 
export const LEDS_ANIMATION_CYCLE = 6000;
// LEDS animation step in milliseconds
export const LEDS_ANIMATION_STEP = 80;
// Default color for LEDs
export const LEDS_COLOR_DEFAULT = "#000000";
// Default number of hotkeys max
export const DEFAULT_NUM_HOTKEYS_MAX = 11;
// max number of key binding per button
export const NUM_BIND_KEY_PER_BUTTON_MAX = 3;
// max length of profile name
export const PROFILE_NAME_MAX_LENGTH = 20;


export enum Platform {
    XINPUT = "XINPUT",
    PS4 = "PS4",
    SWITCH = "SWITCH",
}

export const PlatformList = Object.values(Platform);

export const PlatformLabelMap = new Map<Platform, { label: string, description: string }>([
    [Platform.XINPUT, { 
        label: "Xbox", 
        description: "Xbox controller" 
    }],
    [Platform.PS4, { 
        label: "PlayStation 4", 
        description: "PlayStation 4 controller" 
    }],
    [Platform.SWITCH, { 
        label: "Nintendo Switch", 
        description: "Nintendo Switch controller" 
    }],
]);

export enum GameControllerButton {
    DPAD_UP = "DPAD_UP",
    DPAD_DOWN = "DPAD_DOWN",
    DPAD_LEFT = "DPAD_LEFT",
    DPAD_RIGHT = "DPAD_RIGHT",
    L1 = "L1",
    R1 = "R1",
    L2 = "L2",
    R2 = "R2",
    L3 = "L3",
    R3 = "R3",
    B1 = "B1",
    B2 = "B2",
    B3 = "B3",
    B4 = "B4",
    S1 = "S1",
    S2 = "S2",
    A1 = "A1",
    A2 = "A2",
}

export const GameControllerButtonList = Object.values(GameControllerButton);



export const XInputButtonMap = new Map<GameControllerButton, string>([
    [GameControllerButton.DPAD_UP, "DPAD_UP"],
    [GameControllerButton.DPAD_DOWN, "DPAD_DOWN"],
    [GameControllerButton.DPAD_LEFT, "DPAD_LEFT"],
    [GameControllerButton.DPAD_RIGHT, "DPAD_RIGHT"],
    [GameControllerButton.S1, "BACK"],
    [GameControllerButton.S2, "START"],
    [GameControllerButton.A1, "HOME"],
    [GameControllerButton.L1, "LB"],
    [GameControllerButton.R1, "RB"],
    [GameControllerButton.L2, "LT"],
    [GameControllerButton.R2, "RT"],
    [GameControllerButton.L3, "LEFT_THUMB"],
    [GameControllerButton.R3, "RIGHT_THUMB"],
    [GameControllerButton.B1, "A"],
    [GameControllerButton.B2, "B"],
    [GameControllerButton.B3, "X"],
    [GameControllerButton.B4, "Y"],
]);

export const PS4ButtonMap = new Map<GameControllerButton, string>([
    [GameControllerButton.DPAD_UP, "DPAD_UP"],
    [GameControllerButton.DPAD_DOWN, "DPAD_DOWN"],
    [GameControllerButton.DPAD_LEFT, "DPAD_LEFT"],
    [GameControllerButton.DPAD_RIGHT, "DPAD_RIGHT"],
    [GameControllerButton.S1, "TOUCHPAD"],
    [GameControllerButton.S2, "START"],
    [GameControllerButton.A1, "HOME"],
    [GameControllerButton.L1, "L1"],
    [GameControllerButton.R1, "R1"],
    [GameControllerButton.L2, "L2"],
    [GameControllerButton.R2, "R2"],
    [GameControllerButton.L3, "LEFT_THUMB"],
    [GameControllerButton.R3, "RIGHT_THUMB"],
    [GameControllerButton.B1, "SOUTH"],
    [GameControllerButton.B2, "EAST"],
    [GameControllerButton.B3, "WEST"],
    [GameControllerButton.B4, "NORTH"],
]);

export const SwitchButtonMap = new Map<GameControllerButton, string>([
    [GameControllerButton.DPAD_UP, "DPAD_UP"],
    [GameControllerButton.DPAD_DOWN, "DPAD_DOWN"],
    [GameControllerButton.DPAD_LEFT, "DPAD_LEFT"],
    [GameControllerButton.DPAD_RIGHT, "DPAD_RIGHT"],
    [GameControllerButton.S1, "MINUS"],
    [GameControllerButton.S2, "PLUS"],
    [GameControllerButton.A1, "HOME"],
    [GameControllerButton.A2, "CAPTURE"],
    [GameControllerButton.L1, "L"],
    [GameControllerButton.R1, "R"],
    [GameControllerButton.L2, "ZL"],
    [GameControllerButton.R2, "ZR"],
    [GameControllerButton.L3, "LEFT_THUMB"],
    [GameControllerButton.R3, "RIGHT_THUMB"],
    [GameControllerButton.B1, "B"],
    [GameControllerButton.B2, "A"],
    [GameControllerButton.B3, "Y"],
    [GameControllerButton.B4, "X"],
]);

export type GameProfileList = {
    defaultId: string;
    maxNumProfiles: number;
    items: GameProfile[];
}

export enum GameSocdMode {
    SOCD_MODE_NEUTRAL = "SOCD_MODE_NEUTRAL", // 中性 up + down = neutral, left + right = neutral
    SOCD_MODE_UP_PRIORITY = "SOCD_MODE_UP_PRIORITY", // 上优先 up + down = up, left + right = neutral
    SOCD_MODE_SECOND_INPUT_PRIORITY = "SOCD_MODE_SECOND_INPUT_PRIORITY", // 第二输入优先 
    SOCD_MODE_FIRST_INPUT_PRIORITY = "SOCD_MODE_FIRST_INPUT_PRIORITY", // 第一输入优先 
    SOCD_MODE_BYPASS = "SOCD_MODE_BYPASS", // 绕过 不做任何处理 所有dpad信号都有效
}

export const GameSocdModeList = Object.values(GameSocdMode);

export const GameSocdModeLabelMap = new Map<GameSocdMode, { label: string, description: string }>([
    [GameSocdMode.SOCD_MODE_UP_PRIORITY, { 
        label: "Up Priority", 
        description: "The first input is prioritized when the two inputs are the same." 
    }],
    [GameSocdMode.SOCD_MODE_NEUTRAL, { 
        label: "Neutral", 
        description: "The first input is prioritized when the two inputs are different." 
    }],
    [GameSocdMode.SOCD_MODE_SECOND_INPUT_PRIORITY, { 
        label: "Sec Input Priority", 
        description: "The second input is prioritized when the two inputs are different." 
    }],
    [GameSocdMode.SOCD_MODE_FIRST_INPUT_PRIORITY, { 
        label: "First Input Priority", 
        description: "The first input is prioritized when the two inputs are different." 
    }],
    [GameSocdMode.SOCD_MODE_BYPASS, { 
        label: "Bypass", 
        description: "Bypass the SOCD mode and use the first input." 
    }],
]);

export enum HotkeyAction {
    None = "None",
    LedsEffectStyleNext = "LedsEffectStyleNext",
    LedsEffectStylePrev = "LedsEffectStylePrev",
    LedsBrightnessUp = "LedsBrightnessUp",
    LedsBrightnessDown = "LedsBrightnessDown",
    LedsEnableSwitch = "LedsEnableSwitch",
    CalibrationMode = "CalibrationMode",
    WebConfigMode = "WebConfigMode",
    XInputMode = "XInputMode",
    PS4Mode = "PS4Mode",
    NSwitchMode = "NSwitchMode",
    SystemReboot = "SystemReboot",
}

export const HotkeyActionList = Object.values(HotkeyAction);

export const HotkeyActionLabelMap = new Map<HotkeyAction, { label: string, description: string }>([
    [HotkeyAction.None, { 
        label: "None", 
        description: "No action" 
    }],
    [HotkeyAction.LedsEffectStyleNext, { 
        label: "Next LED Effect", 
        description: "Switch to next LED effect style" 
    }],
    [HotkeyAction.LedsEffectStylePrev, { 
        label: "Previous LED Effect", 
        description: "Switch to previous LED effect style" 
    }],
    [HotkeyAction.LedsBrightnessUp, { 
        label: "Increase Brightness", 
        description: "Increase LED brightness" 
    }],
    [HotkeyAction.LedsBrightnessDown, { 
        label: "Decrease Brightness", 
        description: "Decrease LED brightness" 
    }],
    [HotkeyAction.LedsEnableSwitch, { 
        label: "Toggle LEDs", 
        description: "Enable/Disable LEDs" 
    }],
    [HotkeyAction.WebConfigMode, { 
        label: "Web Config Mode", 
        description: "Enter web configuration mode" 
    }],
    [HotkeyAction.XInputMode, { 
        label: "XInput Mode", 
        description: "Switch to XInput mode" 
    }],
    [HotkeyAction.PS4Mode, { 
        label: "PS4 Mode", 
        description: "Switch to PS4 mode" 
    }],
    [HotkeyAction.NSwitchMode, { 
        label: "Nintendo Switch Mode", 
        description: "Switch to Nintendo Switch mode" 
    }],
    [HotkeyAction.SystemReboot, { 
        label: "System Reboot", 
        description: "Reboot the system" 
    }],
]);

export type Hotkey = {
    key: number,
    action: HotkeyAction,
    isLocked?: boolean,
}

export interface KeysConfig {
    inputMode?: Platform;
    socdMode?: GameSocdMode;
    invertXAxis?: boolean;
    invertYAxis?: boolean;
    fourWayMode?: boolean;
    keyMapping?: {
        [key in GameControllerButton]?: number[];
    };
}

export interface GameProfile {
    id: string;
    name?: string;
    keysConfig?: KeysConfig;
    triggerConfigs?: {
        isAllBtnsConfiguring?: boolean;
        triggerConfigs?: RapidTriggerConfig[];
    };
    ledsConfigs?: {
        ledEnabled: boolean;
        ledsEffectStyle: LedsEffectStyle;
        ledColors: string[];
        ledBrightness: number;
    };
}

export type ADCButton = {
    virtualPin: number,
    magnettization: number,
    topPosition: number,
    bottomPosition: number,
}

export type GPIOButton = {
    virtualPin: number,
}

export interface GamepadConfig {
    version?: number;
    defaultProfileId?: string;
    isCalibrateCompleted?: boolean;
    numProfilesMax?: number;
    ADCButtons?: ADCButton[];
    GPIOButtons?: GPIOButton[];
    profiles?: GameProfile[];
    hotkeys?: Hotkey[];
}

export enum LedsEffectStyle {
    STATIC = "STATIC",
    BREATHING = "BREATHING",
    // WAVE = "WAVE",
}

export type RapidTriggerConfig = {
    topDeadzone: number;
    bottomDeadzone: number;
    pressAccuracy: number;
    releaseAccuracy: number;
}

export const LedsEffectStyleList = Object.values(LedsEffectStyle);

export const LedsEffectStyleLabelMap = new Map<LedsEffectStyle, { label: string, description: string, icon: string, hasBackColor2: boolean }>([
    [LedsEffectStyle.STATIC, { label: "Static", description: "Static color", icon: "sun-dim", hasBackColor2: false }],
    [LedsEffectStyle.BREATHING, { label: "Breathing", description: "Breathing color", icon: "activity", hasBackColor2: true }],
]);

export const ledColorsLabel = [ "Front Color", "Back Color 1", "Back Color 2" ];

// UI Text Constants
export const UI_TEXT = {
    // Common Button Labels
    BUTTON_RESET: "Reset",
    BUTTON_SAVE: "Save",
    BUTTON_REBOOT_WITH_SAVING: "Reboot With Saving",
    BUTTON_CANCEL: "Cancel",
    BUTTON_SUBMIT: "Submit",
    BUTTON_CONFIRM: "Confirm",
    
    // Dialog Titles
    DIALOG_REBOOT_CONFIRM_TITLE: "Are you sure?",
    DIALOG_REBOOT_SUCCESS_TITLE: "Reboot successful",
    DIALOG_CREATE_PROFILE_TITLE: "Create New Profile",
    DIALOG_RENAME_PROFILE_TITLE: "Rename Profile",
    
    // Dialog Messages
    DIALOG_REBOOT_CONFIRM_MESSAGE: "Rebooting the system with saving will save the current profile and ending the current session. Are you sure to continue?",
    DIALOG_REBOOT_SUCCESS_MESSAGE: "Rebooting the system with saving is successful. You can now close this window and start enjoying the gaming experience.",
    DIALOG_CREATE_PROFILE_CONFIRM_MESSAGE: "Creating a new profile will create a new profile and ending the current session. Are you sure to continue?",
    DIALOG_RENAME_PROFILE_CONFIRM_MESSAGE: "Renaming the current profile will save the current profile and ending the current session. Are you sure to continue?",
    
    // Error Messages
    ERROR_KEY_ALREADY_BINDED_TITLE: "Key already binded",
    ERROR_KEY_ALREADY_BINDED_DESC: "Please select another key, or unbind the key first",
    
    // Profile Related
    PROFILE_CREATE_DIALOG_TITLE: "Create New Profile",
    PROFILE_DELETE_DIALOG_TITLE: "Delete Profile",
    PROFILE_DELETE_CONFIRM_MESSAGE: "Deleting this profile can not be undone or reverted. Are you sure you want to delete this profile?",
    PROFILE_NAME_LABEL: "Profile Name",
    PROFILE_NAME_PLACEHOLDER: "Enter profile name",
    
    // Settings Labels
    SETTINGS_SOCD_LABEL: "SOCD Mode",
    SETTINGS_PLATFORM_LABEL: "Platform",
    SETTINGS_LEDS_ENABLE_LABEL: "Enable LEDs",
    SETTINGS_LEDS_EFFECT_LABEL: "LED Effect Style",
    SETTINGS_LEDS_BRIGHTNESS_LABEL: "LED Brightness",
    SETTINGS_LEDS_COLOR_FRONT_LABEL: "Front Color",
    SETTINGS_LEDS_COLOR_BACK1_LABEL: "Back Color 1",
    SETTINGS_LEDS_COLOR_BACK2_LABEL: "Back Color 2",
    SETTINGS_KEY_MAPPING_AUTO_SWITCH_LABEL: "Auto Switch",
    SETTINGS_KEY_MAPPING_MANUAL_SWITCH_LABEL: "Manual Switch",
    SETTINGS_RAPID_TRIGGER_ONFIGURING_BUTTON: "Configuring button: ",
    SETTINGS_RAPID_TRIGGER_SELECT_A_BUTTON_TO_CONFIGURE: "Select a button to configure",
    SETTINGS_RAPID_TRIGGER_TOP_DEADZONE_LABEL: "Top Deadzone (mm)",
    SETTINGS_RAPID_TRIGGER_BOTTOM_DEADZONE_LABEL: "Bottom Deadzone (mm)",
    SETTINGS_RAPID_TRIGGER_PRESS_ACCURACY_LABEL: "Press Accuracy (mm)",
    SETTINGS_RAPID_TRIGGER_RELEASE_ACCURACY_LABEL: "Release Accuracy (mm)",

    // Select Value Text
    SELECT_VALUE_TEXT_PLACEHOLDER: "Select action",
    
    // Tooltips
    TOOLTIP_SOCD_MODE: "SOCD (Simultaneous Opposing Cardinal Directions) handling mode",
    TOOLTIP_PLATFORM_MODE: "Select the platform for controller input",
    TOOLTIP_LEDS_ENABLE: "Toggle LED lighting effects",
    TOOLTIP_LEDS_EFFECT: "Choose the LED animation style",
    TOOLTIP_LEDS_BRIGHTNESS: "Adjust the brightness of LEDs",
    TOOLTIP_AUTO_SWITCH: "Auto Switch: Automatically switch the button field when the input key is changed.\nManual Switch: Manually set the active button field.",
    
    // API Response Messages
    API_REBOOT_SUCCESS_MESSAGE: "System is rebooting",
    API_REBOOT_ERROR_MESSAGE: "Failed to reboot system",
    
    // Loading States
    LOADING_MESSAGE: "Loading...",
    
    // Validation Messages
    VALIDATION_PROFILE_NAME_MAX_LENGTH: `Profile name cannot exceed ${PROFILE_NAME_MAX_LENGTH} characters`,
    VALIDATION_PROFILE_NAME_REQUIRED: "Profile name is required",
    VALIDATION_PROFILE_NAME_CANNOT_BE_SAME_AS_CURRENT_PROFILE_NAME: "Profile name cannot be the same as the current profile name",
    VALIDATION_PROFILE_NAME_ALREADY_EXISTS: "Profile name already exists",
    VALIDATION_PROFILE_NAME_SPECIAL_CHARACTERS: "Profile name cannot contain special characters",
    
    // Keys Settings
    SETTINGS_KEYS_TITLE: "KEYS SETTINGS",
    SETTINGS_KEYS_HELPER_TEXT: `- Input Mode: The input mode of the game controller.\n- Key Mapping: The mapping relationship between the Hitbox buttons and the Game Controller buttons.`,

    // LEDs Settings
    SETTINGS_LEDS_TITLE: "LEDS SETTINGS",
    SETTINGS_LEDS_HELPER_TEXT: "The LED effect style, colors, and brightness can be customized here.\n- Static: The LEDs are always on with the same color.\n- Breathing: The LEDs breath with the two colors.\n- Front Color: The color of the LEDs when the button is pressed.\n- back Color: The color of the LEDs based on the effect.",

    // Rapid Trigger Settings
    SETTINGS_RAPID_TRIGGER_TITLE: "RAPID TRIGGER SETTINGS",
    SETTINGS_RAPID_TRIGGER_HELPER_TEXT: "The rapid trigger settings can be customized here.\n- Top Deadzone: The distance from the top of the trigger to the deadzone.\n- Bottom Deadzone: The distance from the bottom of the trigger to the deadzone.\n- Press Accuracy: The accuracy of the trigger when pressed.\n- Release Accuracy: The accuracy of the trigger when released.",
    SETTINGS_RAPID_TRIGGER_CONFIGURE_ALL: "Configure all buttons at once",

    // Hotkeys Settings
    SETTINGS_HOTKEYS_TITLE: "HOTKEYS SETTINGS",
    SETTINGS_HOTKEYS_HELPER_TEXT: `Configure up to ${DEFAULT_NUM_HOTKEYS_MAX} hotkeys for quick access to various functions.\n- Click on the hotkey field and press the desired key on the hitbox to bind the hotkey.\n- Choice the hotkey action from the dropdown list.\n- Locked hotkeys are used for web configuration mode because this function is required.`,

    // Profile Select
    PROFILE_SELECT_CREATE_BUTTON: "Create New Profile",
    PROFILE_SELECT_RENAME_BUTTON: "Rename Profile",
    PROFILE_SELECT_DELETE_BUTTON: "Delete Profile",
    PROFILE_SELECT_MENU_BUTTON: "Profile Menu",
    PROFILE_SELECT_RENAME_DIALOG_TITLE: "Rename Profile",
    PROFILE_SELECT_RENAME_FIELD_LABEL: "Profile Name",
    PROFILE_SELECT_RENAME_FIELD_PLACEHOLDER: "Enter new profile name",
    PROFILE_SELECT_DELETE_CONFIRM_MESSAGE: "Deleting this profile can not be undone or reverted. Are you sure you want to delete this profile?",
    PROFILE_SELECT_VALIDATION_SPECIAL_CHARS: "Profile name cannot contain special characters",
    PROFILE_SELECT_VALIDATION_LENGTH: `Profile name length must be between 1 and ${PROFILE_NAME_MAX_LENGTH} characters, current length is {0}`,
    PROFILE_SELECT_VALIDATION_SAME_NAME: "Profile name cannot be the same as the current profile name",
    PROFILE_SELECT_VALIDATION_EXISTS: "Profile name already exists",

    // Key Mapping Field
    KEY_MAPPING_KEY_PREFIX: "KEY-",

    // Key Mapping Fieldset
    KEY_MAPPING_ERROR_ALREADY_BINDED_TITLE: "Key already binded",
    KEY_MAPPING_ERROR_ALREADY_BINDED_DESC: "Please select another key",
    KEY_MAPPING_ERROR_MAX_KEYS_TITLE: "Max number of key binding per button reached",
    KEY_MAPPING_ERROR_MAX_KEYS_DESC: "Please unbind some keys first",
    KEY_MAPPING_INFO_UNBIND_FROM_OTHER_TITLE: "Key already binded on other button",
    KEY_MAPPING_INFO_UNBIND_FROM_OTHER_DESC: "Unbinded from [ {0} ] button and Rebinded to [ {1} ] button",

    // Keys Settings
    SETTINGS_KEYS_INVERT_X_AXIS: "Invert X Axis",
    SETTINGS_KEYS_INVERT_Y_AXIS: "Invert Y Axis",
    SETTINGS_KEYS_FOURWAY_MODE: "FourWay Mode",
    SETTINGS_KEYS_FOURWAY_MODE_TOOLTIP: "FourWay Mode: Enable the four-way mode of the Dpad, which means the Dpad will be treated as a four-way direction pad.\n(Only available when the input mode is Switch)",
    SETTINGS_KEYS_MAPPING_TITLE: "Key Mapping",

    // Settings Layout
    SETTINGS_TAB_KEYS: "Keys Setting",
    SETTINGS_TAB_LEDS: "LEDs Setting",
    SETTINGS_TAB_RAPID_TRIGGER: "Rapid Trigger",
    SETTINGS_TAB_HOTKEYS: "Hotkeys Setting",
    SETTINGS_TAB_FIRMWARE: "Firmware",

    // Keys Settings
    SETTINGS_KEYS_INPUT_MODE_TITLE: "Input Mode Choice",
    SETTINGS_KEYS_SOCD_MODE_TITLE: "SOCD Mode Choice",
    SETTINGS_KEYS_PLATFORM_MODE_TOOLTIP: "Platform Mode: Select the platform for controller input",
    SETTINGS_KEYS_SOCD_MODE_TOOLTIP: "SOCD Mode: Select the SOCD (Simultaneous Opposing Cardinal Directions) handling mode",

    // LEDs Settings
    SETTINGS_LEDS_EFFECT_STYLE_CHOICE: "LED Effect Style Choice",
    SETTINGS_LEDS_STATIC_LABEL: "Static",
    SETTINGS_LEDS_STATIC_DESC: "Static color",
    SETTINGS_LEDS_BREATHING_LABEL: "Breathing",
    SETTINGS_LEDS_BREATHING_DESC: "Breathing color",
    SETTINGS_LEDS_COLORS_LABEL: "LED Colors",
    SETTINGS_LEDS_FRONT_COLOR: "Front Color",
    SETTINGS_LEDS_BACK_COLOR1: "Back Color 1",
    SETTINGS_LEDS_BACK_COLOR2: "Back Color 2",

    // Unsaved Changes Warning
    UNSAVED_CHANGES_WARNING_TITLE: "Are you sure?",
    UNSAVED_CHANGES_WARNING_MESSAGE: "You have unsaved changes. If you leave without saving, your changes will be lost.",

    // Hotkeys Actions
    HOTKEY_ACTION_NONE: "None",
    HOTKEY_ACTION_WEB_CONFIG: "Web Config Mode",
    HOTKEY_ACTION_LEDS_ENABLE: "LEDs Enable/Disable",
    HOTKEY_ACTION_LEDS_EFFECT_NEXT: "LEDs Effect Next",
    HOTKEY_ACTION_LEDS_EFFECT_PREV: "LEDs Effect Previous",
    HOTKEY_ACTION_LEDS_BRIGHTNESS_UP: "LEDs Brightness Up",
    HOTKEY_ACTION_LEDS_BRIGHTNESS_DOWN: "LEDs Brightness Down",
    HOTKEY_ACTION_CALIBRATION_MODE: "Calibration Mode",
    HOTKEY_ACTION_XINPUT_MODE: "XInput Mode",
    HOTKEY_ACTION_PS4_MODE: "PlayStation 4 Mode",
    HOTKEY_ACTION_NSWITCH_MODE: "Nintendo Switch Mode",
    HOTKEY_ACTION_SYSTEM_REBOOT: "System Reboot",
} as const;

export const UI_TEXT_ZH = {
    // 通用按钮文案
    BUTTON_RESET: "重置",
    BUTTON_SAVE: "保存",
    BUTTON_REBOOT_WITH_SAVING: "保存并重启",
    BUTTON_CANCEL: "取消",
    BUTTON_SUBMIT: "确定",
    BUTTON_CONFIRM: "确认",
    
    // 对话框标题
    DIALOG_REBOOT_CONFIRM_TITLE: "确认重启?",
    DIALOG_REBOOT_SUCCESS_TITLE: "重启成功",
    DIALOG_CREATE_PROFILE_TITLE: "创建新配置",
    DIALOG_RENAME_PROFILE_TITLE: "重命名配置",
    
    // 对话框消息
    DIALOG_REBOOT_CONFIRM_MESSAGE: "保存并重启系统将会保存当前配置并结束当前会话。是否确认继续？",
    DIALOG_REBOOT_SUCCESS_MESSAGE: "系统重启成功。您现在可以关闭此窗口并开始享受游戏体验。",
    DIALOG_CREATE_PROFILE_CONFIRM_MESSAGE: "创建新配置将会结束当前会话。是否确认继续？",
    DIALOG_RENAME_PROFILE_CONFIRM_MESSAGE: "重命名当前配置将会保存当前配置并结束当前会话。是否确认继续？",
    
    // 错误消息
    ERROR_KEY_ALREADY_BINDED_TITLE: "按键已被绑定",
    ERROR_KEY_ALREADY_BINDED_DESC: "请选择其他按键，或先解绑当前按键",
    
    // 配置相关
    PROFILE_CREATE_DIALOG_TITLE: "创建新配置",
    PROFILE_DELETE_DIALOG_TITLE: "删除配置",
    PROFILE_DELETE_CONFIRM_MESSAGE: "删除此配置后将无法恢复。是否确认删除？",
    PROFILE_NAME_LABEL: "配置名称",
    PROFILE_NAME_PLACEHOLDER: "请输入配置名称",
    
    // 设置标签
    SETTINGS_SOCD_LABEL: "SOCD模式",
    SETTINGS_PLATFORM_LABEL: "平台",
    SETTINGS_LEDS_ENABLE_LABEL: "启用LED",
    SETTINGS_LEDS_EFFECT_LABEL: "LED效果样式",
    SETTINGS_LEDS_BRIGHTNESS_LABEL: "LED亮度",
    SETTINGS_LEDS_COLOR_FRONT_LABEL: "前置颜色",
    SETTINGS_LEDS_COLOR_BACK1_LABEL: "背景颜色1",
    SETTINGS_LEDS_COLOR_BACK2_LABEL: "背景颜色2",
    SETTINGS_KEY_MAPPING_AUTO_SWITCH_LABEL: "自动切换",
    SETTINGS_KEY_MAPPING_MANUAL_SWITCH_LABEL: "手动切换",
    SETTINGS_RAPID_TRIGGER_ONFIGURING_BUTTON: "正在配置按键: ",
    SETTINGS_RAPID_TRIGGER_SELECT_A_BUTTON_TO_CONFIGURE: "请选择要配置的按键",
    SETTINGS_RAPID_TRIGGER_TOP_DEADZONE_LABEL: "顶部死区(毫米)",
    SETTINGS_RAPID_TRIGGER_BOTTOM_DEADZONE_LABEL: "底部死区(毫米)",
    SETTINGS_RAPID_TRIGGER_PRESS_ACCURACY_LABEL: "按下精度(毫米)",
    SETTINGS_RAPID_TRIGGER_RELEASE_ACCURACY_LABEL: "释放精度(毫米)",
    
    // 选择值文本
    SELECT_VALUE_TEXT_PLACEHOLDER: "选择动作",
    
    // 工具提示
    TOOLTIP_SOCD_MODE: "SOCD(同时按下相对方向键)处理模式",
    TOOLTIP_PLATFORM_MODE: "选择控制器输入平台",
    TOOLTIP_LEDS_ENABLE: "开启/关闭LED灯光效果",
    TOOLTIP_LEDS_EFFECT: "选择LED动画样式",
    TOOLTIP_LEDS_BRIGHTNESS: "调整LED亮度",
    TOOLTIP_AUTO_SWITCH: "自动切换：输入按键改变时自动切换按键区域\n手动切换：手动设置活动按键区域",
    
    // API响应消息
    API_REBOOT_SUCCESS_MESSAGE: "系统正在重启",
    API_REBOOT_ERROR_MESSAGE: "系统重启失败",
    
    // 加载状态
    LOADING_MESSAGE: "加载中...",
    
    // 验证消息
    VALIDATION_PROFILE_NAME_MAX_LENGTH: `配置名称不能超过${PROFILE_NAME_MAX_LENGTH}个字符`,
    VALIDATION_PROFILE_NAME_REQUIRED: "配置名称不能为空",
    VALIDATION_PROFILE_NAME_CANNOT_BE_SAME_AS_CURRENT_PROFILE_NAME: "新配置名称不能与当前配置名称相同",
    VALIDATION_PROFILE_NAME_ALREADY_EXISTS: "配置名称已存在",
    VALIDATION_PROFILE_NAME_SPECIAL_CHARACTERS: "配置名称不能包含特殊字符",
    
    // 按键设置
    SETTINGS_KEYS_TITLE: "按键设置",
    SETTINGS_KEYS_HELPER_TEXT: `- 输入模式：游戏控制器的输入模式\n- 按键映射：Hitbox按键与游戏控制器按键的映射关系`,
    
    // LED设置
    SETTINGS_LEDS_TITLE: "LED设置",
    SETTINGS_LEDS_HELPER_TEXT: "可以在这里自定义LED效果样式、颜色和亮度。\n- 静态：LED始终以相同颜色亮起\n- 呼吸：LED在两种颜色之间呼吸\n- 前置颜色：按键按下时的LED颜色\n- 背景颜色：基于效果的LED颜色",
    
    // 快速触发设置
    SETTINGS_RAPID_TRIGGER_TITLE: "快速触发设置",
    SETTINGS_RAPID_TRIGGER_HELPER_TEXT: "可以在这里自定义快速触发设置。\n- 顶部死区：扳机顶部到死区的距离\n- 底部死区：扳机底部到死区的距离\n- 按下精度：扳机按下时的精度\n- 释放精度：扳机释放时的精度",
    SETTINGS_RAPID_TRIGGER_CONFIGURE_ALL: "同时配置所有按键",
    
    // 热键设置
    SETTINGS_HOTKEYS_TITLE: "热键设置",
    SETTINGS_HOTKEYS_HELPER_TEXT: `最多可以配置${DEFAULT_NUM_HOTKEYS_MAX}个热键来快速访问各种功能。\n- 点击热键区域并在hitbox上按下想要绑定的按键\n- 从下拉列表中选择热键动作\n- 锁定的热键用于网页配置模式，因为这个功能是必需的`,
    
    // 配置选择
    PROFILE_SELECT_CREATE_BUTTON: "创建新配置",
    PROFILE_SELECT_RENAME_BUTTON: "重命名配置",
    PROFILE_SELECT_DELETE_BUTTON: "删除配置",
    PROFILE_SELECT_MENU_BUTTON: "配置菜单",
    PROFILE_SELECT_RENAME_DIALOG_TITLE: "重命名配置",
    PROFILE_SELECT_RENAME_FIELD_LABEL: "配置名称",
    PROFILE_SELECT_RENAME_FIELD_PLACEHOLDER: "请输入新的配置名称",
    PROFILE_SELECT_DELETE_CONFIRM_MESSAGE: "删除此配置后将无法恢复。是否确认删除？",
    PROFILE_SELECT_VALIDATION_SPECIAL_CHARS: "配置名称不能包含特殊字符",
    PROFILE_SELECT_VALIDATION_LENGTH: `配置名称长度必须在1到${PROFILE_NAME_MAX_LENGTH}个字符之间，当前长度为{0}`,
    PROFILE_SELECT_VALIDATION_SAME_NAME: "配置名称不能与当前配置名称相同",
    PROFILE_SELECT_VALIDATION_EXISTS: "配置名称已存在",

    // Key Mapping Field
    KEY_MAPPING_KEY_PREFIX: "按键-",

    // Key Mapping Fieldset
    KEY_MAPPING_ERROR_ALREADY_BINDED_TITLE: "按键已被绑定",
    KEY_MAPPING_ERROR_ALREADY_BINDED_DESC: "请选择其他按键",
    KEY_MAPPING_ERROR_MAX_KEYS_TITLE: "已达到每个按键的最大绑定数",
    KEY_MAPPING_ERROR_MAX_KEYS_DESC: "请先解绑一些按键",
    KEY_MAPPING_INFO_UNBIND_FROM_OTHER_TITLE: "按键已在其他按钮上绑定",
    KEY_MAPPING_INFO_UNBIND_FROM_OTHER_DESC: "已从 [ {0} ] 按钮解绑并重新绑定到 [ {1} ] 按钮",

    // Keys Settings
    SETTINGS_KEYS_INVERT_X_AXIS: "反转X轴",
    SETTINGS_KEYS_INVERT_Y_AXIS: "反转Y轴",
    SETTINGS_KEYS_FOURWAY_MODE: "四方向模式",
    SETTINGS_KEYS_FOURWAY_MODE_TOOLTIP: "四方向模式：启用十字键的四方向模式，这意味着十字键将被视为四方向键盘。\n(仅在输入模式为Switch时可用)",
    SETTINGS_KEYS_MAPPING_TITLE: "按键映射",

    // Settings Layout
    SETTINGS_TAB_KEYS: "按键设置",
    SETTINGS_TAB_LEDS: "LED设置",
    SETTINGS_TAB_RAPID_TRIGGER: "快速触发",
    SETTINGS_TAB_HOTKEYS: "热键设置",
    SETTINGS_TAB_FIRMWARE: "固件",

    // Keys Settings
    SETTINGS_KEYS_INPUT_MODE_TITLE: "输入模式选择",
    SETTINGS_KEYS_SOCD_MODE_TITLE: "SOCD模式选择",
    SETTINGS_KEYS_PLATFORM_MODE_TOOLTIP: "平台模式：选择控制器输入平台",
    SETTINGS_KEYS_SOCD_MODE_TOOLTIP: "SOCD模式：选择SOCD(同时按下相对方向键)处理模式",

    // LEDs Settings
    SETTINGS_LEDS_EFFECT_STYLE_CHOICE: "LED效果样式选择",
    SETTINGS_LEDS_STATIC_LABEL: "静态",
    SETTINGS_LEDS_STATIC_DESC: "固定颜色",
    SETTINGS_LEDS_BREATHING_LABEL: "呼吸",
    SETTINGS_LEDS_BREATHING_DESC: "呼吸效果",
    SETTINGS_LEDS_COLORS_LABEL: "LED颜色",
    SETTINGS_LEDS_FRONT_COLOR: "前置颜色",
    SETTINGS_LEDS_BACK_COLOR1: "背景颜色1",
    SETTINGS_LEDS_BACK_COLOR2: "背景颜色2",

    // Unsaved Changes Warning
    UNSAVED_CHANGES_WARNING_TITLE: "确认离开?",
    UNSAVED_CHANGES_WARNING_MESSAGE: "您有未保存的更改。如果离开而不保存，您的更改将会丢失。",

    // Hotkeys Actions
    HOTKEY_ACTION_NONE: "无",
    HOTKEY_ACTION_WEB_CONFIG: "网页配置模式",
    HOTKEY_ACTION_LEDS_ENABLE: "开启/关闭LED",
    HOTKEY_ACTION_LEDS_EFFECT_NEXT: "下一个LED效果",
    HOTKEY_ACTION_LEDS_EFFECT_PREV: "上一个LED效果",
    HOTKEY_ACTION_LEDS_BRIGHTNESS_UP: "LED亮度增加",
    HOTKEY_ACTION_LEDS_BRIGHTNESS_DOWN: "LED亮度减少",
    HOTKEY_ACTION_CALIBRATION_MODE: "按键校准模式",
    HOTKEY_ACTION_XINPUT_MODE: "XInput模式",
    HOTKEY_ACTION_PS4_MODE: "PlayStation 4模式",
    HOTKEY_ACTION_NSWITCH_MODE: "Nintendo Switch模式",
    HOTKEY_ACTION_SYSTEM_REBOOT: "系统重启",
} as const;

