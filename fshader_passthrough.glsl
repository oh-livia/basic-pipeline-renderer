// the simplest fragment shader: get the color (from the vertex shader - 
// probably interpolated from values specified in the vertex shader - and
// just pass them through to the render:
// 

// on Mac, you may have to say this: 
// "varying vec4 color;" instead of this:
//in vec4 color;
varying vec4 color;

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
    
    vec4 mat_amb = fmat_amb;
    vec4 mat_diff = fmat_diff;
    vec4 mat_spec = fmat_spec;
   
    vec4 blackCol = vec4(0, 0, 0, 1);
    vec4 blackSpec = vec4(0.5, 0.5, 0.5, 1);
    vec4 whiteCol = vec4(1, 1, 1, 1);
    vec4 whiteSpec = vec4(0.8, 0.8, 0.8, 1);

    if (fchecker > 0.)
    {   
        mat_amb = vec4(0.1, 0.1, 0.1, 1);
        
        float x = (vecPos[0])*fdivisions;
        float y = (vecPos[1])*fdivisions;
        float z = (vecPos[2])*fdivisions;
        
        int mx = int(mod(floor(x), 2.));
        int my = int(mod(floor(y), 2.));
        int mz = int(mod(floor(z), 2.));
        
        if (mx == my)
        {
            mat_diff = blackCol;
            mat_spec = blackSpec;
        }
        else
        {
            mat_diff = whiteCol;
            mat_spec = whiteSpec;
        }
        
        if (mz == 0)
        {
            if (mat_diff == blackCol)
            {
                mat_diff = whiteCol;
                mat_spec = whiteSpec;
            }
            else
            {
                mat_diff = blackCol;
                mat_spec = blackSpec;
            }
        }
        
    }

    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;

    vec4 norm_light_pos = normalize(flight_pos);
    vec4 view_pos = normalize(fcamEye);
    vec4 halfvec = normalize(norm_light_pos+view_pos);
    
    ambient_color = product(mat_amb, flight_amb);
    
    
    float dd = dot(norm_light_pos, normals);
    
    if(dd>0.0)
        diffuse_color = dd*product(flight_diff, mat_diff);
    else 
        diffuse_color = vec4(0.0, 0.0, 0.0, 1.0);
    
    dd = dot(halfvec, normals);
    if(dd > 0.0)
        specular_color = exp(fmat_shine*log(dd))*product(flight_spec, mat_spec);
    else
        specular_color = vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragColor = ambient_color + diffuse_color + specular_color;


} 

