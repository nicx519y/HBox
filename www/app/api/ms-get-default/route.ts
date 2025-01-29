import { NextResponse } from 'next/server';
import { getDefaultMapping } from '../data/adc_store';
import { ADCBtnsError } from '@/types/adc';

export async function GET() {
    try {
        const name = getDefaultMapping();
        if(!name) {
            return NextResponse.json({
                errNo: ADCBtnsError.MAPPING_NOT_FOUND,
                data: { 
                    name: null
                 }
            });
        } else {
            return NextResponse.json({
                errNo: ADCBtnsError.SUCCESS,
                data: { 
                    name
                }
            });
        }
    } catch {
        return NextResponse.json(
            { errNo: 1, errorMessage: 'Internal server error' },
            { status: 500 }
        );
    }
} 