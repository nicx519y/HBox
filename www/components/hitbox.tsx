'use client';

import { useEffect, useState, useRef } from "react";
import { LEDS_ANIMATION_CYCLE, LedsEffectStyle } from "@/types/gamepad-config";
import { Color, parseColor, Box } from '@chakra-ui/react';
import styled from "styled-components";
import { useGamepadConfig } from "@/contexts/gamepad-config-context";
import { useColorMode } from "./ui/color-mode";

const StyledSvg = styled.svg`
  width: 800px;
  height: 650px;
  position: relative;
`;

/**
 * StyledCircle
 * @param props 
 *  $color: string - 颜色
 *  $opacity: number - 透明度
 *  $interactive: boolean - 是否可交互  
 * @returns 
 */
const StyledCircle = styled.circle<{
    $color?: string;
    $opacity?: number;
    $interactive?: boolean;
    $highlight?: boolean;
}>`
  stroke: 'gray';
  stroke-width: 1px;
  cursor: ${props => props.$interactive ? 'pointer' : 'default'};
  pointer-events: ${props => props.$interactive ? 'auto' : 'none'};
  opacity: ${props => props.$opacity};
  fill: ${props => props.$color ?? 'transparent'};
  stroke: ${props => props.$highlight ? 'yellowgreen' : 'gray'};
  stroke-width: ${props => props.$highlight ? '2px' : '1px'};
  filter: ${props => props.$highlight ? 'drop-shadow(0 0 2px rgba(154, 205, 50, 0.8))' : 'none'};

  &:hover {
    stroke-width: ${props => props.$interactive ? '2px' : '1px'};
    stroke: ${props => props.$interactive ? '#ccc' : 'gray'};
    filter: ${props => props.$interactive ? 'drop-shadow(0 0 10px rgba(204, 204, 204, 0.8))' : 'none'};
  }

  &:active {
    stroke-width: ${props => props.$interactive ? '2px' : '1px'};
    stroke: ${props => props.$interactive ? 'yellowgreen' : 'gray'};
    filter: ${props => props.$interactive ? 'drop-shadow(0 0 15px rgba(154, 205, 50, 0.9))' : 'none'};
  }
`;

const StyledPath = styled.path`
  fill: none;
  stroke: gray;
  stroke-linecap: round;
  stroke-linejoin: round;
  stroke-width: 1px;
  filter: drop-shadow(0 0 1px rgba(128, 128, 128, 0.5));
`;

const StyledFrame = styled.rect`
  fill: none;
  stroke: gray;
  stroke-width: 1px;
  filter: drop-shadow(0 0 5px rgba(204, 204, 204, 0.8));
`;

/**
 * StyledText
 * @returns 
 */
const StyledText = styled.text`
  text-align: center;
  font-family: "Helvetica", cursive;
  font-size: .9rem;
  cursor: default;
  pointer-events: none;
`;

const btnPosList = [
    { x: 376.2, y: 379.8, r: 36 },
    { x: 299.52, y: 352.44, r: 28.63 },
    { x: 452.88, y: 352.44, r: 28.63 },
    { x: 304.97, y: 182.0, r: 28.63 },
    { x: 239.31, y: 170.56, r: 28.63 },
    { x: 359.52, y: 220.35, r: 28.63 },
    { x: 330.43, y: 120.46, r: 28.63 },
    { x: 435.24, y: 226.76, r: 28.63 },
    { x: 404.82, y: 163.22, r: 28.63 },
    { x: 398.52, y: 92.67, r: 28.63 },
    { x: 493.2, y: 186.48, r: 28.63 },
    { x: 462.78, y: 122.94, r: 28.63 },
    { x: 559.8, y: 162.36, r: 28.63 },
    { x: 529.43, y: 98.67, r: 28.63 },
    { x: 630.36, y: 156.06, r: 28.63 },
    { x: 599.94, y: 92.52, r: 28.63 },
    { x: 184.03, y: 46.03, r: 11.37 },
    { x: 140.02, y: 46.03, r: 11.37 },
    { x: 96.01, y: 46.03, r: 11.37 },
    { x: 51.99, y: 46.03, r: 11.37 },
];

const btnFrameRadiusDistance = 3;

const btnLen = btnPosList.length;

const lerpColor = (color1: Color, color2: Color, t: number) => {

    const r = Math.round(color1.getChannelValue('red') + (color2.getChannelValue('red') - color1.getChannelValue('red')) * t);
    const g = Math.round(color1.getChannelValue('green') + (color2.getChannelValue('green') - color1.getChannelValue('green')) * t);
    const b = Math.round(color1.getChannelValue('blue') + (color2.getChannelValue('blue') - color1.getChannelValue('blue')) * t);

    return parseColor(`rgb(${r}, ${g}, ${b})`);

};

// 反转颜色
function invertColor(color: Color) {
    const r = color.getChannelValue('red');
    const g = color.getChannelValue('green');
    const b = color.getChannelValue('blue');
    const invertedR = 255 - r;
    const invertedG = 255 - g;
    const invertedB = 255 - b;
    return parseColor(`rgb(${invertedR}, ${invertedG}, ${invertedB})`);
}



/**
 * Hitbox
 * @param props 
 * onClick: (id: number) => void - 点击事件
 * colorEnabled: boolean - 是否用颜色
 * color1: string - 默认颜色
 * color2: string - 呼吸颜色
 * brightness: number - 亮度
 * interactiveIds: number[] - 可交互按钮id列表  
 * @returns 
 */
export default function Hitbox(props: {
    onClick?: (id: number) => void,
    hasLeds?: boolean,
    colorEnabled?: boolean,
    frontColor?: Color,
    backColor1?: Color,
    backColor2?: Color,
    brightness?: number,
    effectStyle?: LedsEffectStyle,
    interactiveIds?: number[],
    highlightIds?: number[],
}) {

    const { colorMode } = useColorMode()
    const [defaultFrontColor, setDefaultFrontColor] = useState(parseColor("#ffffff"));
    const [colorList, setColorList] = useState<Color[]>(Array(btnLen).fill(defaultFrontColor));
    const frontColorRef = useRef(props.frontColor ?? defaultFrontColor);
    const backColor1Ref = useRef(props.backColor1 ?? defaultFrontColor);
    const backColor2Ref = useRef(props.backColor2 ?? defaultFrontColor);
    const colorEnabledRef = useRef(props.colorEnabled ?? false);
    const effectStyleRef = useRef(props.effectStyle ?? LedsEffectStyle.STATIC);
    const pressedButtonListRef = useRef(Array(btnLen).fill(-1));
    const defaultFrontColorRef = useRef(defaultFrontColor);

    const { contextJsReady, setContextJsReady } = useGamepadConfig();

    /**
     * 根据颜色模式设置默认颜色
     */
    useEffect(() => {
        if (colorMode === "dark") {
            setDefaultFrontColor(parseColor("#000000"));
            defaultFrontColorRef.current = parseColor("#000000");
            setColorList(Array(btnLen).fill(parseColor("#000000")));
        } else {
            setDefaultFrontColor(parseColor("#ffffff"));
            defaultFrontColorRef.current = parseColor("#ffffff");
            setColorList(Array(btnLen).fill(parseColor("#ffffff")));
        }
    }, [colorMode]);

    const handleClick = (event: React.MouseEvent<SVGElement>) => {
        const target = event.target as SVGElement;
        if (!target.id || !target.id.startsWith("btn-")) return;
        const id = Number(target.id.replace("btn-", ""));
        if (id === Number.NaN || !(props.interactiveIds?.includes(id) ?? false)) return;
        if (event.type === "mousedown") {
            props.onClick?.(id);
            pressedButtonListRef.current[id] = 1;
        } else if (event.type === "mouseup") {
            props.onClick?.(-1);
            pressedButtonListRef.current[id] = -1;
        }
    };

    const handleLeave = (event: React.MouseEvent<SVGElement>) => {
        const target = event.target as SVGElement;
        if (!target.id || !target.id.startsWith("btn-")) return;
        const id = Number(target.id.replace("btn-", ""));
        if (id === Number.NaN || !(props.interactiveIds?.includes(id) ?? false)) return;
        if (event.type === "mouseleave") {
            pressedButtonListRef.current[id] = -1;
        }
    }

    /**
     * 获取按钮表面颜色
     * @param index 
     * @returns 
     */
    const getBtnFontColor = (index: number): string => {
        if ([16, 17, 18, 19].includes(index)) {
            return defaultFrontColor.toString('css');
        }
        return colorList[index]?.toString('css') ?? defaultFrontColor.toString('css');
    }

    /**
     * 获取按钮字体颜色 是按钮表面颜色的反色
     * @param index 
     * @returns 
     */
    const getTextColor = (index: number) => {
        return invertColor(parseColor(getBtnFontColor(index))).toString('css');
    }

    /**
     * 初始化显示状态
     */
    useEffect(() => {
        setContextJsReady(true);
    }, []);

    /**
     * 更新按钮颜色
     */
    useEffect(() => {

        if (!props.hasLeds) {
            return;
        }

        let animationFrameId: number;
        let timer: number;

        // 更新按钮颜色 
        const updateColors = () => {

            const now = new Date().getTime();
            const deltaTime = now - timer;
            const progress = deltaTime % LEDS_ANIMATION_CYCLE / LEDS_ANIMATION_CYCLE;
            const newColors = colorList.map((_, index) => {
                if (1 === pressedButtonListRef.current[index] && colorEnabledRef.current) {
                    return frontColorRef.current;
                } else if (colorEnabledRef.current) {
                    if (effectStyleRef.current === LedsEffectStyle.BREATHING) {
                        const t = Math.sin(progress * Math.PI);
                        return lerpColor(backColor1Ref.current as Color, backColor2Ref.current as Color, t);
                    } else if (effectStyleRef.current === LedsEffectStyle.STATIC) {
                        return backColor1Ref.current;
                    }
                }
                return defaultFrontColorRef.current;
            });

            setColorList(newColors as Color[]);
            animationFrameId = requestAnimationFrame(updateColors);
        };

        timer = new Date().getTime();
        // 启动动画 
        animationFrameId = requestAnimationFrame(updateColors);

        return () => {
            cancelAnimationFrame(animationFrameId);
            timer = 0;
        };


    }, []);

    useEffect(() => {
        const brightness = props.brightness ?? 100;
        if (props.frontColor) {
            const r = props.frontColor.getChannelValue('red');
            const g = props.frontColor.getChannelValue('green');
            const b = props.frontColor.getChannelValue('blue');
            frontColorRef.current = parseColor(`rgba(${r}, ${g}, ${b}, ${brightness / 100})`);
        }
    }, [props.frontColor, props.brightness]);

    useEffect(() => {
        const brightness = props.brightness ?? 100;
        if (props.backColor1) {
            const r = props.backColor1.getChannelValue('red');
            const g = props.backColor1.getChannelValue('green');
            const b = props.backColor1.getChannelValue('blue');
            backColor1Ref.current = parseColor(`rgba(${r}, ${g}, ${b}, ${brightness / 100})`);
        }
    }, [props.backColor1, props.brightness]);

    useEffect(() => {
        const brightness = props.brightness ?? 100;
        if (props.backColor2) {
            const r = props.backColor2.getChannelValue('red');
            const g = props.backColor2.getChannelValue('green');
            const b = props.backColor2.getChannelValue('blue');
            backColor2Ref.current = parseColor(`rgba(${r}, ${g}, ${b}, ${brightness / 100})`);
        }
    }, [props.backColor2, props.brightness]);

    useEffect(() => {
        effectStyleRef.current = props.effectStyle ?? LedsEffectStyle.STATIC;
    }, [props.effectStyle]);

    useEffect(() => {
        colorEnabledRef.current = props.colorEnabled ?? true;
    }, [props.colorEnabled]);

    return (
        <Box display={contextJsReady ? "block" : "none"} >
            <StyledSvg xmlns="http://www.w3.org/2000/svg"
                onMouseDown={handleClick}
                onMouseUp={handleClick}
            >
                <title>hitbox</title>
                <StyledFrame x="0.36" y="0.36" width="787.82" height="507.1" rx="10" />

                {/* 渲染按钮边框路径 */}
                <StyledPath d="
              M328.23,220.98 a10,10,0,0,0-4.27-8
              M323.97,212.95 a10,10,0,0,0-9-1.26
              M276.83,195.89 a31.22,31.22,0,0,0,38.31,15.87
              M276.83,195.89 a10,10,0,0,0-7.25-5.48
              M269.59,190.24 a9.94,9.94,0,0,0-8.7,2.66
              M267.32,157.1 a31.23,31.23,0,1,0-6.36,36.06
              M267.31,157.09 a10,10,0,0,0,7.26,5.48
              M274.55,162.57 a10,10,0,0,0,8.7-2.66
              M297.69,151.87 a31.28,31.28,0,0,0-14.51,8
              M297.7,152.01 a10,10,0,0,0,6.91-5.9
              M304.61,146.13 a10,10,0,0,0-.72-9.07
              M337.53,151.07 a31.22,31.22,0,1,0-33.83-14
              M337.53,150.93 a10,10,0,0,0-6.92,5.9
              M330.62,156.81 a10,10,0,0,0,.71,9.08
              M336.15,181.77 a31.32,31.32,0,0,0-4.63-15.89
              M335.97,181.67 a10,10,0,0,0,4.26,8
              M340.22,189.67 a10,10,0,0,0,9,1.26
              M328.24,220.89 a31.23,31.23,0,1,0,21-30"
                />

                {/* 渲染按钮外框 */}
                {btnPosList.map((item, index) => {
                    const radius = item.r + btnFrameRadiusDistance;
                    if (![3, 4, 5, 6].includes(index)) {
                        return (
                            <StyledCircle
                                id={`btn-${index}`}
                                key={index}
                                cx={item.x}
                                cy={item.y}
                                r={radius}
                                $interactive={false}
                                $highlight={false}
                            />
                        )
                    }
                })}

                {/* 渲染按钮 */}
                {btnPosList.map((item, index) => (
                    <StyledCircle
                        id={`btn-${index}`}
                        key={index}
                        cx={item.x}
                        cy={item.y}
                        r={item.r}
                        onMouseLeave={handleLeave}
                        $color={[16, 17, 18, 19].includes(index) ? defaultFrontColor.toString('css') : colorList[index]?.toString('css') ?? defaultFrontColor.toString('css')}
                        $opacity={1}
                        $interactive={props.interactiveIds?.includes(index) ?? false}
                        $highlight={props.highlightIds?.includes(index) ?? false}
                    />
                ))}

                {/* 渲染按钮文字 */}
                {btnPosList.map((item, index) => (
                    <StyledText
                        textAnchor="middle"
                        dominantBaseline="middle"
                        key={index}
                        x={item.x}
                        y={index < btnLen - 4 ? item.y : item.y + 30}
                        fill={getTextColor(index)}
                    >
                        {index !== btnLen - 1 ? index + 1 : "Fn"}
                    </StyledText>
                ))}
            </StyledSvg>
        </Box>
    );
}