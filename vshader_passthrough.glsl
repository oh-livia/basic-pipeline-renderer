attribute vec4 vPosition;
attribute vec4 vNormal;
attribute vec4 vColor;
uniform mat4 rotY;
uniform mat4 rotX;
uniform mat4 camera;
uniform mat4 persp;

uniform vec4 camEye;
uniform vec4 light_pos;
uniform vec4 light_amb;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 mat_amb;
uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_shine;

uniform float checker;
uniform float divisions;

//varying vec4 color;
varying vec4 normals;
varying vec4 fcamEye;
varying vec4 flight_pos;
varying vec4 flight_amb;
varying vec4 flight_diff;
varying vec4 flight_spec;
varying vec4 fmat_amb;
varying vec4 fmat_diff;
varying vec4 fmat_spec;
varying float fmat_shine;

varying float fchecker;
varying float fdivisions;
varying vec4 vecPos;

vec4 product(vec4 a, vec4 b)
{
    return vec4(a[0]*b[0], a[1]*b[1], a[2]*b[2], a[3]*b[3]);
}

void main() 
{
    
    normals = normalize(vNormal);
    fcamEye = camEye;
    flight_pos = light_pos;
    flight_amb = light_amb;
    flight_diff = light_diff;
    flight_spec = light_spec;
    
    fmat_amb = mat_amb;
    fmat_diff = mat_diff;
    fmat_spec = mat_spec;
    fmat_shine = mat_shine;
/* 
    vec4 blackCol = vec4(0, 0, 0, 1);
    vec4 blackSpec = vec4(0.5, 0.5, 0.5, 1);
    vec4 whiteCol = vec4(1, 1, 1, 1);
    vec4 whiteSpec = vec4(0.8, 0.8, 0.8, 1);
    
    if (checker)
    {   
        fmat_amb = vec4(0.1, 0.1, 0.1, 1);
        fmat_diff = vNormal;
      
        float x = (vPosition[0] + 1.)*divisions;
        float y = (vPosition[1] + 1.)*divisions;
        float z = (vPosition[2] + 1.)*divisions;
        
        int mx = int(mod(x, 2.));
        int my = int(mod(y, 2.));
        int mz = int(mod(z, 2.));
        
        if (mx == my)
        {
            fmat_diff = blackCol;
            fmat_spec = blackSpec;
        }
        else
        {
            fmat_diff = whiteCol;
            fmat_spec = whiteSpec;
        }
        
        if (mz == 0)
        {
            if (fmat_diff == blackCol)
            {
                fmat_diff = whiteCol;
                fmat_spec = whiteSpec;
            }
            else
            {
                fmat_diff = blackCol;
                fmat_spec = blackSpec;
            }
        }

    }
    
  
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
    
    vec4 norm_light_pos = normalize(flight_pos);
    vec4 view_pos = normalize(fcamEye);
    vec4 halfvec = normalize(norm_light_pos+view_pos);
    
    ambient_color = product(fmat_amb, flight_amb);
    
    
    float dd = dot(norm_light_pos, normals);
    
    if(dd>0.0)
        diffuse_color = dd*product(flight_diff, fmat_diff);
    else 
        diffuse_color = vec4(0.0, 0.0, 0.0, 1.0);
    
    dd = dot(halfvec, normals);
    if(dd > 0.0)
        specular_color = exp(fmat_shine*log(dd))*product(flight_spec, fmat_spec);
    else
        specular_color = vec4(0.0, 0.0, 0.0, 1.0);
    
    color = ambient_color + diffuse_color + specular_color;
*/


    fchecker = checker;
    fdivisions = divisions;
    vecPos = vPosition;

  
    gl_Position = persp * camera * rotX * rotY * vPosition;
    
} 
