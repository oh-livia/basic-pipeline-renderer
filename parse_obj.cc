
#include "parse_obj.h"

#include <sstream>
#include <fstream>
#include "bezObj.h"

extern std::vector<bezObj *> objs;

using namespace std;


// Given the name of a wavefront (OBJ) file that consists JUST of
// vertices, triangles, and comments, read it into the tris and verts
// vectors.
//
// tris is a vector of ints that is 3*n long, where n is the number of
// triangles. The ith triangle has vertex indexes 3*i, 3*i+1, and 3*i+2.
//
// The ith triangle has vertices verts[3*i], verts[3*i+1], and verts[3*i+2],
// given in counterclockwise order with respect to the surface normal
//
void read_wavefront_file (const char *file)//, std::vector< int > &tris, std::vector< float > &verts)
{
    
    ifstream in(file);
    char buffer[1025];
    string cmd;
    int noObj;
    int objOn = 0;
    int lineAt;
    bool readUVNum = false;
    int u_deg, v_deg;
    vec4 **points;
    
    int largestCoord; //This is to determine if an object has to be scaled to fit 2x2
    
    
    for (int line=1; in.good(); line++) {
        in.getline(buffer,1024);
        buffer[in.gcount()]=0;
        
        istringstream iss (buffer);
        if (line == 1){
            iss >> noObj;
            readUVNum = true;
        }
        else if (readUVNum)
        {
            iss >> u_deg >> v_deg;
            bezObj *bezierObj = new bezObj();
            bezierObj->setUV(u_deg, v_deg);
            objs.push_back(bezierObj);
            
            points = new vec4*[v_deg+1];
            for (int i = 0; i < v_deg+1; i++)
                points[i] = new vec4[u_deg+1];
            lineAt = v_deg;
            
            largestCoord = 0;
            
            readUVNum = false;
        }
        else {
            for (int i = 0; i < u_deg+1; i++)
            {
                double x, y, z;
                iss >> x >> y >> z;
                
                //Save largest value
                if (abs(x) > largestCoord)
                    largestCoord = abs(x);
                if (abs(y) > largestCoord)
                    largestCoord = abs(y);
                if (abs(z) > largestCoord)
                    largestCoord = abs(z);
                
                points[lineAt][i] = vec4(x, y, z, 1);
            }
            --lineAt;
            if (lineAt < 0)
            {
                if (largestCoord > 1)
                {
                    for (int i = 0; i < u_deg+1; ++i)
                        for (int j = 0; j < v_deg+1; ++j)
                        {
                            points[j][i] = (0.99/largestCoord) * points[j][i];
                            points[j][i].w = 1.;
                        }
                }
                objs[objOn]->setPoints(points);
                objOn++;
                readUVNum = true;
            }
        }
        
    }
    
    
    
    in.close();

    
/*    
    tris.clear();
    verts.clear();
    
    ifstream in(file);
    char buffer[1025];
    string cmd;
    
    
    for (int line=1; in.good(); line++) {
        in.getline(buffer,1024);
        buffer[in.gcount()]=0;
                
        cmd="";
        
        istringstream iss (buffer);
        
        iss >> cmd;
        
        if (cmd[0]=='#' or cmd.empty()) {
            // ignore comments or blank lines
            continue;
        } 
        else if (cmd=="v") {
            // got a vertex:
            
            // read in the parameters:
            double pa, pb, pc;
            iss >> pa >> pb >> pc;
 
            verts.push_back (pa);
            verts.push_back (pb);
            verts.push_back (pc);
         } 
        else if (cmd=="f") {
            // got a face (triangle)
            
            // read in the parameters:
            int i, j, k;
            iss >> i >> j >> k;
            
            // vertex numbers in OBJ files start with 1, but in C++ array
            // indices start with 0, so we're shifting everything down by
            // 1
            tris.push_back (i-1);
            tris.push_back (j-1);
            tris.push_back (k-1);
        } 
        else {
            std::cerr << "Parser error: invalid command at line " << line << std::endl;
        }
        
     }
    
    in.close();
    
    std::cout << "found this many tris, verts: " << tris.size () / 3.0 << "  " 
              << verts.size () / 3.0 << std::endl;
  */   
     
}

