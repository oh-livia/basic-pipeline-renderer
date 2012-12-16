#include <iostream>
#include "bezObj.h"

using namespace std;

bezObj::bezObj()
{
    u_deg = 0;
    v_deg = 0;
}

bezObj::bezObj(int u, int v)
{
    u_deg = u;
    v_deg = v;
    uvs = new vec4*[v_deg+1];
    for (int i = 0; i < v_deg+1; ++i)
    {
        uvs[i] = new vec4[u_deg+1];
    }
    computedPoints = new vec4*[v_deg+1];
    for (int i = 0; i < v_deg+1; ++i)
    {
        computedPoints[i] = new vec4[u_deg+1];
    }
    computedNormals = new vec4*[v_deg+1];
    for (int i = 0; i < v_deg+1; ++i)
    {
        computedNormals[i] = new vec4[u_deg+1];
    }
}

void bezObj::setUV(int u, int v)
{
    u_deg = u;
    v_deg = v;
    uvs = new vec4*[v_deg+1];
    for (int i = 0; i < v_deg+1; ++i)
    {
        uvs[i] = new vec4[u_deg+1];
    }
    computedPoints = new vec4*[v_deg+1];
    for (int i = 0; i < v_deg+1; ++i)
    {
        computedPoints[i] = new vec4[u_deg+1];
    } 
    computedNormals = new vec4*[v_deg+1];
    for (int i = 0; i < v_deg+1; ++i)
    {
        computedNormals[i] = new vec4[u_deg+1];
    }
}

bezObj::~bezObj()
{
}

void bezObj::setPoints(vec4 **points)
{
    for (int i = 0; i < v_deg+1; i++)
    {
        for (int j = 0; j < u_deg+1; j++)
        {
            uvs[i][j] = *((*(points+i))+j);
        }
        cout << endl;
    }
}

void bezObj::computePoints(int samples)
{
    t = samples;
    int vSamples = v_deg+t;
    int uSamples = u_deg+t;
    
    computedPoints = new vec4*[vSamples];
    for (int i = 0; i < vSamples; ++i)
    {
        computedPoints[i] = new vec4[uSamples];
    } 
    computedNormals = new vec4*[vSamples];
    for (int i = 0; i < vSamples; ++i)
    {
        computedNormals[i] = new vec4[uSamples];
    }
    
    vec4 **computedUtan = new vec4*[vSamples];
    for (int i = 0; i < vSamples; ++i)
    {
        computedUtan[i] = new vec4[uSamples];
    }
    vec4 **computedVtan = new vec4*[vSamples];
    for (int i = 0; i < vSamples; ++i)
    {
        computedVtan[i] = new vec4[uSamples];
    }
    
    vec4 *tempCurve;
    for (int uSam = 0; uSam < uSamples; ++uSam)
    {
        float tu = uSam/float(uSamples-1);
        tempCurve = new vec4[v_deg+1];
        for (int v = 0; v < v_deg+1; ++v)
        {
            vec4 *tempBez = new vec4[u_deg+1];
            for (int u = 0; u < u_deg+1; ++u)
                tempBez[u] = uvs[v][u];
            tempCurve[v] = computePoint(tempBez, tu, u_deg, false);
        }
        for (int vSam = 0; vSam < vSamples; ++vSam)
        {
            float tv = vSam/float(vSamples-1);
            computedPoints[vSam][uSam] = computePoint(tempCurve, tv, v_deg, false);
            computedVtan[vSam][uSam] = computePoint(tempCurve, tv, v_deg, true);
        }
    }
    
    for (int vSam = 0; vSam < vSamples; ++vSam)
    {
        float tv = vSam/float(vSamples-1);
        tempCurve = new vec4[u_deg+1];
        for (int u = 0; u < u_deg+1; ++u)
        {
            vec4 *tempBez = new vec4[v_deg+1];
            for (int v = 0; v < v_deg+1; ++v)
                tempBez[v] = uvs[v][u];
            tempCurve[u] = computePoint(tempBez, tv, v_deg, false);
        }
        for (int uSam = 0; uSam < uSamples; ++uSam)
        {
            float tu = uSam/float(uSamples-1);
            computedUtan[vSam][uSam] = computePoint(tempCurve, tu, u_deg, true);
            computedNormals[vSam][uSam] = normalize(cross(computedVtan[vSam][uSam], computedUtan[vSam][uSam]));
            computedNormals[vSam][uSam].w = 1;
        }
    }

}

vec4 bezObj::computePoint(vec4 *pointArray, float t, int degree, bool isNormal)
{
    vec4 *temp = new vec4[degree+1];
    
    for (int i = 0; i < degree+1; i++)
        temp[i] = pointArray[i];

    int times = degree;
    while(times > 0)
    {
        if ((times == 1) && (isNormal == true))
        {
            temp[0] = temp[1] - temp[0];
            temp[0].w = 0;
            times = 0;
        }
        else{
            for (int i = 0; i < times; ++i)
            {
                vec4 comp = (1-t)*temp[i] + t*temp[i+1];
                temp[i] = comp;
                temp[i].w = 0;
            }
            --times;
        }
    }
    return temp[0];
}

int bezObj::numVertices()
{
    return ((v_deg+t-1)*(u_deg+t-1)*6);
}

void bezObj::makeTriangles(vec4 *vertices, vec4 *normals)
{
    int k = 0;
    for (int i = 0; i < u_deg+t-1; ++i)
    {
        for (int j = 0; j < v_deg+t-1; ++j)
        {
            vertices[6*k]     = computedPoints[j][i];
            vertices[6*k + 1] = computedPoints[j+1][i];
            vertices[6*k + 2] = computedPoints[j][i+1];
            vertices[6*k + 3] = computedPoints[j][i+1];
            vertices[6*k + 4] = computedPoints[j+1][i];
            vertices[6*k + 5] = computedPoints[j+1][i+1];
            
            normals[6*k]     = computedNormals[j][i];
            normals[6*k + 1] = computedNormals[j+1][i];
            normals[6*k + 2] = computedNormals[j][i+1];
            normals[6*k + 3] = computedNormals[j][i+1];
            normals[6*k + 4] = computedNormals[j+1][i];
            normals[6*k + 5] = computedNormals[j+1][i+1];
            
            ++k;
        }
    }
}



