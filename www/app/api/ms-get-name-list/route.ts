import { NextResponse } from 'next/server';
import { getMappingNameList } from '../data/adc_store';
import { ADCBtnsError } from '@/types/adc';

export async function GET() {
    try {
        const nameList = getMappingNameList();

        console.log("api: getMappingNameList", nameList);
        
        return NextResponse.json({
            errNo: ADCBtnsError.SUCCESS,
            data: { nameList }
        });
    } catch (error) {
        return NextResponse.json(
            { errNo: 1, errorMessage: 'Internal server error' },
            { status: 500 }
        );
    }
} 