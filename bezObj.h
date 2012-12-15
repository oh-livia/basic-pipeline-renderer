//
//  bezObj.h
//  glrender
//
//  Created by Olivia Winn on 12/12/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef glrender_bezObj_h
#define glrender_bezObj_h

#include "amath.h"

class bezObj
{
private:
    int u_deg, v_deg;
    vec4 **uvs;
    int t;
    vec4 **computedPoints;
    vec4 **computedNormals;
    vec4 computePoint(vec4 *pointArray, float t, int degree, bool isNormal);
public:
    bezObj();
    ~bezObj();
    bezObj(int u, int v);
    void setUV(int u, int v);
    void setPoints(vec4 **points);
    void computePoints(int t);
    int numVertices();
    void makeTriangles(vec4 *vertices, vec4 *normals);
};

#endif
