#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform sampler2D imgInput;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

const int kernelSize = 11;

// Gaussian kernel values
const float kernel[kernelSize] = float[](
0.05, 0.05, 0.1, 0.1, 0.1, 0.2, 0.1, 0.1, 0.1, 0.05, 0.05
);

void main()
{
    ivec2 tID = ivec2(gl_GlobalInvocationID.xy);
    vec4 result = vec4(0.0);

    for (int i = -kernelSize/2; i <= kernelSize/2; ++i) {
        ivec2 neighborID = tID + ivec2(i, 0);
        vec4 neighborColor = texelFetch(imgInput, neighborID, 0);
        
        // Get the corresponding value from the kernel
        float weight = kernel[i + kernelSize/2];
        
        // Accumulate weighted color values
        result += weight * neighborColor;
    }

    // Store the result in the output image
    imageStore(imgOutput, tID, result);
}
