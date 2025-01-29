import { NextResponse } from 'next/server';
import { getMarkingStatus, startMarking } from '../data/adc_store';

export async function POST(request: Request) {
    try {
        const body = await request.json();
        const { name } = body;

        // 参数验证
        if (!name) {
            return NextResponse.json(
                { errNo: 1, errorMessage: 'Invalid parameters' },
                { status: 400 }
            );
        }

        const error = startMarking(name);
        return NextResponse.json({ errNo: error, data: {
            status: getMarkingStatus()
        } });
    } catch (error) {
        return NextResponse.json(
            { errNo: 1, errorMessage: 'Internal server error' },
            { status: 500 }
        );
    }
} 