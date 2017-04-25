#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include <rpc/xdr.h>


typedef struct {
	char skin;
	int x;
 	int y;

}Object;

bool_t xdr_object(XDR* pt_xdr, Object* object);

#endif
