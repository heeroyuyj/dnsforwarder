#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "addresslist.h"
#include "common.h"
#include "utils.h"

int AddressList_Init(AddressList *a)
{
	if( a == NULL )
	{
		return 0;
	}

	if( Array_Init(&(a -> AddressList), sizeof(struct _Address), 8, FALSE, NULL) != 0 )
	{
		return -1;
	}

	a -> Counter = 0;
	return 0;
}


int AddressList_Add(AddressList *a, struct _Address	*Addr)
{
	if( a == NULL )
	{
		return -1;
	}

	if( Array_PushBack(&(a -> AddressList), Addr, NULL) < 0 )
	{
		return -1;
	} else {
		return 0;
	}

}

sa_family_t AddressList_ConvertToAddressFromString(struct _Address *Out, const char *Addr_Port, int DefaultPort)
{
	sa_family_t	Family;

	memset(Out, 0, sizeof(struct _Address));

	Family = GetAddressFamily(Addr_Port);
	Out -> family = Family;

	switch( Family )
	{
		case AF_INET6:
			{
				char		Addr[LENGTH_OF_IPV6_ADDRESS_ASCII] = {0};
				in_port_t	Port;
				const char	*PortPos;

				memset(Addr, 0, sizeof(Addr));

				PortPos = strchr(Addr_Port, ']');
				if( PortPos == NULL )
				{
					return AF_UNSPEC;
				}

				PortPos = strchr(PortPos, ':');
				if( PortPos == NULL )
				{
					sscanf(Addr_Port, "[%s]", Addr);
					Port = DefaultPort;
				} else {
					int	Port_warpper;

					sscanf(Addr_Port + 1, "%[^]]", Addr);
					sscanf(PortPos + 1, "%d", &Port_warpper);
					Port = Port_warpper;
				}

				Out -> Addr.Addr6.sin6_family = Family;
				Out -> Addr.Addr6.sin6_port = htons(Port);

				IPv6AddressToNum(Addr, &(Out -> Addr.Addr6.sin6_addr));

				return AF_INET6;
			}
			break;

		case AF_INET:
			{
				char		Addr[] = "xxx.xxx.xxx.xxx";
				in_port_t	Port;
				const char	*PortPos;

				memset(Addr, 0, sizeof(Addr));

				PortPos = strchr(Addr_Port, ':');
				if( PortPos == NULL )
				{
					sscanf(Addr_Port, "%s", Addr);
					Port = DefaultPort;
				} else {
					int Port_warpper;
					sscanf(Addr_Port, "%[^:]", Addr);
					sscanf(PortPos + 1, "%d", &Port_warpper);
					Port = Port_warpper;
				}
				FILL_ADDR4(Out -> Addr.Addr4, Family, Addr, Port);

				return AF_INET;
			}
			break;

		default:
			return AF_UNSPEC;
			break;
	}
}

int AddressList_Add_From_String(AddressList *a, const char *Addr_Port)
{
	struct	_Address	Tmp;

	if( AddressList_ConvertToAddressFromString(&Tmp, Addr_Port, 53) == AF_UNSPEC )
	{
		return -1;
	}

	return AddressList_Add(a, &Tmp);

}

int AddressList_Advance(AddressList *a)
{
	if( a == NULL )
	{
		return 0;
	}

	return (a -> Counter)++;
}

struct sockaddr *AddressList_GetOneBySubscript(AddressList *a, sa_family_t *family, int Subscript)
{
	struct _Address *Result;

	if( a == NULL )
	{
		return 0;
	}

	Result = (struct _Address *)Array_GetBySubscript(&(a -> AddressList), Subscript);
	if( Result == NULL )
	{
		return NULL;
	} else {
		if( family != NULL )
		{
			*family = Result -> family;
		}
		return (struct sockaddr *)&(Result -> Addr);
	}
}

struct sockaddr *AddressList_GetOne(AddressList *a, sa_family_t *family)
{
	return AddressList_GetOneBySubscript(a, family, a -> Counter % Array_GetUsed(&(a -> AddressList)));
}
