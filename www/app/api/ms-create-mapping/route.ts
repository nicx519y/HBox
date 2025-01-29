import { NextResponse } from 'next/server';
import { createMapping } from '../data/adc_store';

export async function POST(request: Request) {
    try {
        const body = await request.json();
        const { name, length, step } = body;

        // 参数验证
        if (!name || !length || !step) {
            return NextResponse.json(
                { errNo: 1, errorMessage: 'Invalid parameters' },
                { status: 400 }
            );
        }

        const errNo = createMapping(name, length, step);
        return NextResponse.json({ errNo, data: {} });
    } catch {
        return NextResponse.json(
            { errNo: 1, errorMessage: 'Internal server error' },
            { status: 500 }
        );  
    }
} 