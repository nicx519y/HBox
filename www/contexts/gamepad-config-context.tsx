'use client';

import { createContext, useContext, useState, useEffect, useMemo } from 'react';
import { GameProfile, 
        LedsEffectStyle, 
        Platform, GameSocdMode, 
        GameControllerButton, Hotkey, RapidTriggerConfig, GameProfileList } from '@/types/gamepad-config';
import { ADCBtnsError, StepInfo, ADCValuesMapping } from '@/types/adc';

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
    setError: (error: string | null) => void;
    rebootSystem: () => Promise<void>;
    // ADC Mapping 相关
    defaultMappingName: string;
    markingStatus: StepInfo;
    mappingNameList: string[];
    activeMapping: ADCValuesMapping | null;
    fetchMappingNameList: () => Promise<void>;
    fetchDefaultMapping: () => Promise<void>;
    fetchActiveMapping: (name: string) => Promise<void>;
    createMapping: (name: string, length: number, step: number) => Promise<void>;
    deleteMapping: (name: string) => Promise<void>;
    updateDefaultMapping: (name: string) => Promise<void>;
    startMarking: (name: string) => Promise<void>;
    stopMarking: () => Promise<void>;
    stepMarking: () => Promise<void>;
    fetchMarkingStatus: () => Promise<void>;
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
    const [defaultMappingName, setDefaultMappingName] = useState<string>("");
    const [mappingNameList, setMappingNameList] = useState<string[]>([]);
    const [markingStatus, setMarkingStatus] = useState<StepInfo>({
        mapping_name: "",
        step: 0,
        length: 0,
        index: 0,
        values: [],
        is_marking: false,
        is_sampling: false,
        is_completed: false
    });
    const [activeMapping, setActiveMapping] = useState<ADCValuesMapping | null>(null);

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
                return Promise.reject(new Error("Failed to fetch default profile"));
            };
            setDefaultProfile(converProfileDetails(data.profileDetails) ?? {});
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch default profile"));
        } finally {
            setIsLoading(false);
        }
    };

    const fetchProfileList = async (): Promise<void> => {
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
                return Promise.reject(new Error("Failed to fetch profile list"))    ;
            };
            setProfileList(data.profileList);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch profile list"));
        } finally {
            setIsLoading(false);
        }
    };

    const fetchHotkeysConfig = async (): Promise<void> => {
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
                return Promise.reject(new Error("Failed to fetch hotkeys config"));
            };
            setHotkeysConfig(data.hotkeysConfig);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch hotkeys config"));
        } finally {
            setIsLoading(false);
        }
    }

    const updateProfileDetails = async (profileId: string, profileDetails: GameProfile): Promise<void> => {
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
                return Promise.reject(new Error("Failed to update profile details"));
            };

            // 如果更新的是 profile 的 name，则需要重新获取 profile list
            if(profileDetails.hasOwnProperty("name") || profileDetails.hasOwnProperty("id")) {
                fetchProfileList();
            } else { // 否则更新 default profile
                setDefaultProfile(converProfileDetails(data.profileDetails) ?? {});
            }
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to update profile details"));
        } finally {
            setIsLoading(false);
        }
    };

    const resetProfileDetails = async (): Promise<void> => {
        await fetchDefaultProfile();
    };

    const createProfile = async (profileName: string): Promise<void> => {
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
                return Promise.reject(new Error("Failed to create profile"));
            };
            setProfileList(data.profileList);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to create profile"));
        } finally {
            setIsLoading(false);
        }
    };

    const deleteProfile = async (profileId: string): Promise<void> => {
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
                return Promise.reject(new Error("Failed to delete profile"));
            };
            setProfileList(data.profileList);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to delete profile"));
        } finally {
            setIsLoading(false);
        }
    };

    const switchProfile = async (profileId: string): Promise<void> => {
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
                return Promise.reject(new Error("Failed to switch profile"));
            };
            setProfileList(data.profileList);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to switch profile"));
        } finally {
            setIsLoading(false);
        }
    };

    const updateHotkeysConfig = async (hotkeysConfig: Hotkey[]): Promise<void> => {
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
                return Promise.reject(new Error("Failed to update hotkeys config"));
            }
            setHotkeysConfig(data.hotkeysConfig);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to update hotkeys config"));
        } finally {
            setIsLoading(false);
        }
    };

    const rebootSystem = async (): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/reboot', {
                method: 'POST',
            });

            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to reboot system"));
            }
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to reboot system"));
        } finally {
            setIsLoading(false);
        }
    };

    const fetchMappingNameList = async (): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-get-name-list', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to fetch mapping name list"));
            }
            setMappingNameList(data.nameList);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch mapping name list"));
        } finally {
            setIsLoading(false);
        }
    };

    const fetchDefaultMapping = async (): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-get-default', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to fetch default mapping"));
            }
            setDefaultMappingName(data.name ?? "");
            return Promise.resolve(data.name);
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch default mapping"));
        } finally {
            setIsLoading(false);
        }
    };

    const createMapping = async (name: string, length: number, step: number): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-create-mapping', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name, length, step }),
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to create mapping"));
            }
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to create mapping"));
        } finally {
            setIsLoading(false);
        }
    };

    const deleteMapping = async (name: string): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-delete-mapping', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name }),
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to delete mapping"));
            }
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to delete mapping"));
        } finally {
            setIsLoading(false);
        }
    };

    const updateDefaultMapping = async (name: string): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-set-default', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name }),
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to set default mapping"));
            }
            setDefaultMappingName(name);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to set default mapping"));
        } finally {
            setIsLoading(false);
        }
    };

    const startMarking = async (name: string): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-mark-mapping-start', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name }),
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to start marking"));
            }

            if(data.status) {
                setMarkingStatus(data.status);
            }

            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to start marking"));
        } finally {
            setIsLoading(false);
        }
    };

    const stopMarking = async (): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-mark-mapping-stop', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to stop marking"));
            }

            if(data.status) {
                setMarkingStatus(data.status);
            }

            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to stop marking"));
        } finally {
            setIsLoading(false);
        }
    };

    const stepMarking = async (): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-mark-mapping-step', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to step marking"));
            }

            if(data.status) {
                setMarkingStatus(data.status);
            }

            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to step marking"));
        } finally {
            setIsLoading(false);
        }
    };

    const fetchMarkingStatus = async (): Promise<void> => {
        try {
            const response = await fetch('/api/ms-get-mark-status', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            const data = await processResponse(response, setError);
            if (!data) {
                return Promise.reject(new Error("Failed to fetch marking status"));
            }

            if(data.status) {
                setMarkingStatus(data.status);
            }

            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch marking status"));
        }
    };

    const fetchActiveMapping = async (name: string): Promise<void> => {
        try {
            setIsLoading(true);
            const response = await fetch('/api/ms-get-mapping', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name }),
            });

            const data = await processResponse(response, setError);

            if (!data) {
                return Promise.reject(new Error("Failed to fetch mapping"));
            }

            setActiveMapping(data.mapping);
            setError(null);
            return Promise.resolve();
        } catch (err) {
            setError(err instanceof Error ? err.message : 'An error occurred');
            return Promise.reject(new Error("Failed to fetch mapping"));
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
            setError,
            rebootSystem,
            // ADC Mapping 相关
            defaultMappingName: defaultMappingName,
            markingStatus,
            mappingNameList,
            activeMapping,
            fetchMappingNameList,
            fetchMarkingStatus,
            updateDefaultMapping,
            fetchDefaultMapping,
            fetchActiveMapping,
            createMapping,
            deleteMapping,
            startMarking,
            stopMarking,
            stepMarking,
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