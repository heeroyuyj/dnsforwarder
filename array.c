#include <string.h>
#include <math.h>
#include "array.h"
#include "utils.h"

/* if it grows down, the InitialCount will be ignored. Otherwise, TheFirstAddress will be ignored. */
int Array_Init(__in Array *a, __in int DataLength, __in int InitialCount, __in BOOL GrowsDown, __in void *TheFirstAddress /* The first means the biggest address*/)
{
	if( InitialCount < 0)
		return 1;

	a -> DataLength = DataLength;
	a -> Used = 0;

	if( GrowsDown == FALSE )
	{
		if( InitialCount > 0 )
		{
			a -> Data = SafeMalloc(DataLength * InitialCount);
			if( a -> Data == NULL )
				return 2;

			memset(a -> Data, 0, DataLength * InitialCount);
		} else {
			a -> Data = NULL;
		}

		a -> Allocated = InitialCount;
	} else {
		a -> Allocated = -1;
		a -> Data = TheFirstAddress;
	}

	return 0;
}
/* Subscript is always non-negative. */
void *Array_GetBySubscript(__in Array *a, __in int Subscript)
{
	if( Subscript >= 0 && Subscript < a -> Used )
	{
		if( a -> Allocated < 0 )
		{
			Subscript *= (-1);
		}

		return (void *)((a -> Data) + (a -> DataLength) * Subscript);
	} else {
		return NULL;
	}
}

/* Subscribe returned */
int Array_PushBack(__in Array *a, __in_opt const void *Data, __in_opt void *Boundary /* Only used by grow down array */)
{
	if( a -> Allocated >= 0 )
	{
		if( a -> Used == a -> Allocated )
		{
			int NewCount = (a -> Allocated) < 2 ? 2 : (a -> Allocated) + (a -> Allocated) / 2;

			if( SafeRealloc((void **)&(a -> Data), NewCount * (a -> DataLength)) != 0 )
			{
				return -1;
			}

			a -> Allocated = NewCount;
		}

		if( Data != NULL )
			memcpy((a -> Data) + (a -> DataLength) * (a -> Used), Data, a -> DataLength);

		return (a -> Used)++;

	} else {
		if( Boundary != NULL && ((a -> Data) + (-1) * (a -> DataLength) * (a -> Used)) < (char *)Boundary )
		{
			return -1;
		} else {
			if( Data != NULL )
			{
				memcpy((a -> Data) + (-1) * (a -> DataLength) * (a -> Used), Data, a -> DataLength);
			}
			return (a -> Used)++;
		}
	}
}


void *Array_SetToSubscript(Array *a, int Subscript, void *Data)
{
	if( a -> Allocated >= 0 )
	{
		if( Subscript >= a -> Allocated )
		{
			if( SafeRealloc((void **)&(a -> Data), (Subscript + 1) * (a -> DataLength)) != 0 )
				return NULL;

			a -> Allocated = Subscript + 1;
		}

		memcpy((a -> Data) + (a -> DataLength) * Subscript, Data, a -> DataLength);

		if( a -> Used < Subscript + 1 )
			a -> Used = Subscript + 1;

		return (a -> Data) + (a -> DataLength) * Subscript;
	} else {
		if( a -> Used < Subscript + 1 )
		{
			a -> Used = Subscript + 1;
		}

		memcpy((a -> Data) + (-1) * (a -> DataLength) * Subscript, Data, a -> DataLength);

		return (a -> Data) + (-1) * (a -> DataLength) * Subscript;
	}
}

void Array_Free(Array *a)
{
	if( a -> Allocated >= 0 )
	{
		SafeFree(a -> Data);
	}
	a -> Data = NULL;
	a -> Used = 0;
	a -> Allocated = 0;
}
