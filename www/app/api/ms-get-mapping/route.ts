import { NextResponse } from 'next/server';
import { getMapping } from '../data/adc_store';
import { ADCBtnsError } from '@/types/adc';

export async function POST(request: Request) {
    try {
        // 从 URL 参数中获取 name
        const { name } = await request.json();

        if (!name) {
            return NextResponse.json({
                errNo: ADCBtnsError.INVALID_PARAMS,
                data: { mapping: null }
            });
        }

        const mapping = getMapping(name);
        if (!mapping) {
            return NextResponse.json({
                errNo: ADCBtnsError.MAPPING_NOT_FOUND,
                data: { mapping: null }
            });
        }

        return NextResponse.json({
            errNo: ADCBtnsError.SUCCESS,
            data: { mapping }
        });
    } catch {
        return NextResponse.json(
            { errNo: 1, errorMessage: 'Internal server error' },
            { status: 500 }
        );
    }
} 