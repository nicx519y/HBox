'use client';

import { Provider } from "@/components/ui/provider"
import StyledComponentsRegistry from '@/lib/registry'
import { SettingsLayout } from '@/components/settings-layout'
import { GamepadConfigProvider, useGamepadConfig } from '@/contexts/gamepad-config-context'
import { Flex, Center, Text, HStack } from '@chakra-ui/react'
import { toaster, Toaster } from "@/components/ui/toaster"
import { LoadingModal } from "@/components/ui/loading-modal"
import { useState, useEffect } from 'react'
import { DialogConfirm } from '@/components/dialog-confirm'
import { DialogForm } from "@/components/dialog-form";
import { DialogCannotClose } from '@/components/dialog-cannot-close'
import { LanguageProvider } from '@/contexts/language-context';
import { LanguageSwitcher } from '@/components/language-switcher'
import { ColorModeSwitcher } from "@/components/color-mode-switcher";

// 创建一个内部组件来使用 context
function AppContent({ children }: { children: React.ReactNode }) {
    const { isLoading } = useGamepadConfig();
    const [showLoading, setShowLoading] = useState(false);
    const { error, setError } = useGamepadConfig();

    // 全局错误处理
    useEffect(() => {
        if (error) {
            toaster.error({
                title: "Error",
                description: error,
            });
            setError(null);
        }
    }, [error]);

    // 全局loading处理
    useEffect(() => {
        let timer: NodeJS.Timeout;
        // 延迟300ms显示loading
        if (!isLoading) {
            timer = setTimeout(() => {
                setShowLoading(false);
            }, 300);
        } else {
            setShowLoading(true);
        }

        return () => {
            // 清理定时器
            if (timer) {
                clearTimeout(timer);
            }
        };
    }, [isLoading]);

    return (
        <Flex
            direction="column"
            height="100vh"
            width="100vw"
            position="relative"
            overflow="auto"
        >
            {/* 添加语言切换按钮 */}
            <HStack
                gap={4}
                position="fixed"
                top={"8px"}
                right={4}
                zIndex={2}
            >
                <LanguageSwitcher />
                <ColorModeSwitcher />
            </HStack>


            {/* 内容区域 */}
            <Flex direction="column" height="100%" zIndex={1}>
                <SettingsLayout>
                    {children}
                </SettingsLayout>
                <Center as="footer" height="40px" borderTop="1px solid" borderColor="rgba(0, 150, 255, 0.15)">
                    <Text fontSize="sm" color="gray.500">
                        © 2024 Hitbox Web Config. All rights reserved.
                    </Text>
                </Center>
            </Flex>
            <Toaster />
            <LoadingModal isOpen={showLoading} />
        </Flex>
    );
}

// 根布局组件
export default function RootLayout({
    children,
}: {
    children: React.ReactNode
}) {
    return (
        <html suppressHydrationWarning style={{ height: '100%' }}>
            <body style={{ height: '100vh', margin: 0 }}>
                <StyledComponentsRegistry>
                    <Provider>
                        <GamepadConfigProvider>
                            <LanguageProvider>
                                <AppContent>
                                    {children}
                                </AppContent>
                                <DialogConfirm />
                                <DialogForm />
                                <DialogCannotClose />
                            </LanguageProvider>
                        </GamepadConfigProvider>
                    </Provider>
                </StyledComponentsRegistry>
            </body>
        </html>
    )
}
