import type { NextConfig } from "next";
import withBundleAnalyzer from '@next/bundle-analyzer';
import TerserPlugin from 'terser-webpack-plugin';

const nextConfig: NextConfig = {
    // output: "export",   // 指定输出模式，export 表示导出静态文件，export 模式下，next 会生成一个 dist 目录，里面包含所有静态文件，使用这个模式的时候 要暂时删除 app/api 
    // distDir: 'build',
    // 忽略 build 错误
    typescript: {
        ignoreBuildErrors: true,
    },
    // 禁用 telemetry   
    telemetry: {
        enabled: false,
    },
    webpack: (config, { isServer }) => {
        if (!isServer) {
            // 禁用代码分割
            config.optimization = {
                minimize: true,
                minimizer: [
                    new TerserPlugin({
                        terserOptions: {
                            compress: {
                                drop_console: true,
                                drop_debugger: true
                            },
                            output: {
                                comments: false,
                            },
                            mangle: true
                        },
                    }),
                ],
                // 关键改动：将所有代码强制打包到一个文件
                concatenateModules: true,
                splitChunks: false,  // 完全禁用代码分割
                runtimeChunk: false,
            };

            // 修改输出配置
            config.output = {
                ...config.output,
                filename: 'static/js/[name].[contenthash].js',
                chunkFilename: 'static/js/[name].[contenthash].js',
                // assetModuleFilename: 'static/media/[name].[hash][ext]',
                // path: paths.appBuild,
                // publicPath: '/',
                // clean: true,
                // library: 'myApp',
                // libraryTarget: 'umd',
                // strictModuleExceptionHandling: true,
                // environment: {
                //     arrowFunction: true,
                //     bigIntLiteral: false,
                //     const: true,
                //     destructuring: true,
                //     dynamicImport: false,
                //     forOf: true,
                //     module: false,   
                // },
                // hashFunction: 'xxhash64',
                // hashDigestLength: 8,
            };
        }
        return config;
    },
    compress: true,
    poweredByHeader: false,
    generateEtags: false,
    async rewrites() {
        return [
            {
                source: '/keys',
                destination: '/',
            },
            {
                source: '/hotkeys',
                destination: '/',
            },
            {
                source: '/leds',
                destination: '/',
            },
            // {
            //     source: '/calibration',
            //     destination: '/',
            // },
            {
                source: '/rapid-trigger',
                destination: '/',
            },
            {
                source: '/firmware',
                destination: '/',
            },
        ];
    },
};

export default withBundleAnalyzer({
    enabled: process.env.ANALYZE === 'true', // 是否启用分析
})(nextConfig);