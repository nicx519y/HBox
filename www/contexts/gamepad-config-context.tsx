'use client';

import { createContext, useContext, useState, useEffect, useMemo } from 'react';
import { GamepadConfig, GameProfile, LedsEffectStyle, Platform, NUM_PROFILES_MAX, GameSocdMode, GameControllerButton, Hotkey, RapidTriggerConfig, GameProfileList } from '@/types/gamepad-config';

interface GamepadConfigContextType {
    contextJsReady: boolean;
    setContextJsReady: (ready: boolean) => void;
    profileList: GameProfileList;
    defaultProfile: GameProfile;
    hotkeysConfig: Hotkey[];
    fetchDefaultProfile: () => Promise<void>;
    fetchProfileList: () => Promise<void>;
    fetchHotkeysConfig: () => Promise<void>;
    updateProfileDetails: (profileId: string, profileDetails: GameProfile) => Promise<void>;
    resetProfileDetails: () => Promise<void>;
    createProfile: (profileName: string) => Promise<void>;
    deleteProfile: (profileId: string) => Promise<void>;
    switchProfile: (profileId: string) => Promise<void>;
    updateHotkeysConfig: (hotkeysConfig: Hotkey[]) => Promise<void>;
    isLoading: boolean;
    error: string | null;
    rebootSystem: () => Promise<void>;
}

const GamepadConfigContext = createContext<GamepadConfigContextType | undefined>(undefined);

/**
 * convert profile details
 * @param profile - GameProfile
 * @returns 
 */
const converProfileDetails = (profile: any) => {
    const newProfile: GameProfile = {
        ...profile,
        keysConfig: {
            inputMode: profile.keysConfig?.inputMode as Platform ?? Platform.XINPUT,
            socdMode: profile.keysConfig?.socdMode as GameSocdMode ?? GameSocdMode.SOCD_MODE_UP_PRIORITY,
            invertXAxis: profile.keysConfig?.invertXAxis as boolean ?? false,
            invertYAxis: profile.keysConfig?.invertYAxis as boolean ?? false,
            fourWayMode: profile.keysConfig?.fourWayMode as boolean ?? false,
            keyMapping: profile.keysConfig?.keyMapping as { [key in GameControllerButton]?: number[] } ?? {},
        },
        ledsConfigs: {
            ledEnabled: profile.ledsConfigs?.ledEnabled as boolean ?? false,
            ledsEffectStyle: profile.ledsConfigs?.ledsEffectStyle as LedsEffectStyle ?? LedsEffectStyle.STATIC,
            ledColors: profile.ledsConfigs?.ledColors as string[] ?? ["#000000", "#000000", "#000000"],
            ledBrightness: profile.ledsConfigs?.ledBrightness as number ?? 100,
        },
        hotkeys: profile.hotkeys as Hotkey[] ?? [],
        triggerConfigs: profile.triggerConfigs as { [key: number]: RapidTriggerConfig } ?? {},
    }
    return newProfile;
}

/**
 * process response
 * @param response - Response
 * @returns 
 */
const processResponse = async (response: Response, setError: (error: string | null) => void) => {
    if (!response.ok) {
        setError(response.statusText);
        return;
    }
    const data = await response.json();
    if(data.errNo) {
        setError(data.errorMessage);
        return;
    }
    return data.data;
}

/**
 * GamepadConfigProvider
 * @param children - React.ReactNode
 * @returns 
 */
export function GamepadConfigProvider({ children }: { children: React.ReactNode }) {
    const [profileList, setProfileList] = useState<GameProfileList>({ defaultId: "", maxNumProfiles: 0, items: [] });
    const [defaultProfile, setDefaultProfile] = useState<GameProfile>({ id: "", name: "" });
    const [isLoading, setIsLoading] = useState(true);
    const [error, setError] = useState<string | null>(null);
    const [hotkeysConfig, setHotkeysConfig] = useState<Hotkey[]>([]);
    const [jsReady, setJsReady] = useState(false);

    const contextJsReady = useMemo(() => {
        return jsReady;
    }, [jsReady]);

    useEffect(() => {
        fetchProfileList();
        fetchHotkeysConfig();
    }, []);

    useEffect(() => {
        if (profileList.defaultId !== "") {
            fetchDefaultProfile();
        }
    }, [profileList]);

    const setContextJsReady = (ready: boolean) => {
        setJsReady(ready);
    }

    const fetchDefaultProfile = async (): Promise<void> => {
        console.log("fetchDefaultProfile");
        try {
            setIsLoading(true);
            const response = await fetch('/api/default-profile', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };
            setDefaultProfile(converProfileDetails(data.profileDetails) ?? {});
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const fetchProfileList = async (): Promise<void> => {
        console.log("fetchProfileList");
        try {
            setIsLoading(true);
            const response = await fetch('/api/profile-list', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            }); 
            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };
            setProfileList(data.profileList);
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const fetchHotkeysConfig = async (): Promise<void> => {
        console.log("fetchHotkeysConfig");
        try {
            setIsLoading(true);
            const response = await fetch('/api/hotkeys-config', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };
            setHotkeysConfig(data.hotkeysConfig);
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    }

    const updateProfileDetails = async (profileId: string, profileDetails: GameProfile): Promise<void> => {
        console.log("updateProfileDetails");
        try {
            setIsLoading(true);
            const response = await fetch('/api/update-profile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ profileId, profileDetails }),
            });

            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };

            // 如果更新的是 profile 的 name，则需要重新获取 profile list
            if(profileDetails.hasOwnProperty("name") || profileDetails.hasOwnProperty("id")) {
                fetchProfileList();
            } else { // 否则更新 default profile
                setDefaultProfile(converProfileDetails(data.profileDetails) ?? {});
            }
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const resetProfileDetails = async (): Promise<void> => {
        console.log("resetProfileDetails");
        await fetchDefaultProfile();
    };

    const createProfile = async (profileName: string): Promise<void> => {
        console.log("createProfile");
        try {
            setIsLoading(true);
            const response = await fetch('/api/create-profile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ profileName }),
            });

            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };
            setProfileList(data.profileList);
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const deleteProfile = async (profileId: string): Promise<void> => {
        console.log("deleteProfile");
        try {
            setIsLoading(true);
            const response = await fetch('/api/delete-profile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ profileId }),
            });

            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };
            setProfileList(data.profileList);
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const switchProfile = async (profileId: string): Promise<void> => {
        console.log("switchProfile");
        try {
            setIsLoading(true);
            const response = await fetch('/api/switch-default-profile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ profileId }),
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return;
            };
            setProfileList(data.profileList);
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const updateHotkeysConfig = async (hotkeysConfig: Hotkey[]): Promise<void> => {
        console.log("updateHotkeysConfig");
        try {
            setIsLoading(true);
            const response = await fetch('/api/update-hotkeys-config', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ hotkeysConfig }),
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return;
            }
            setHotkeysConfig(data.hotkeysConfig);
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    const rebootSystem = async (): Promise<void> => {
        console.log("rebootSystem");
        try {
            setIsLoading(true);
            const response = await fetch('/api/reboot', {
                method: 'POST',
            });

            const data = await processResponse(response, setError);
            if (!data) {
                return;
            }
            
            setError(null);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
        } finally {
            setIsLoading(false);
        }
    };

    return (
        <GamepadConfigContext.Provider value={{
            contextJsReady,
            setContextJsReady,
            profileList,
            defaultProfile,
            hotkeysConfig,
            fetchDefaultProfile,
            fetchProfileList,
            fetchHotkeysConfig,
            updateProfileDetails,
            resetProfileDetails,
            createProfile,
            deleteProfile,
            switchProfile,
            updateHotkeysConfig,
            isLoading,
            error,
            rebootSystem,
        }}>
            {children}
        </GamepadConfigContext.Provider>
    );
}

export function useGamepadConfig() {
    const context = useContext(GamepadConfigContext);
    if (context === undefined) {
        throw new Error('useGamepadConfig must be used within a GamepadConfigProvider');
    }
    return context;
} 