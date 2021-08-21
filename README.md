# DirectX11 Ship Scene

This project is a 3D Scene featuring a ship, water, land and rocks rendered using DirectX 11 and C++. This was my first experience using a programmable pipeline to render 3D objects, so I learned about using Vertex and Pixel shaders to achieve desired effects such as the water.

The application was developed over Semester 1 and 2 for my 'Further Game and Graphical Systems Development' module in my second year at university. The first semester was entirely focussed on rendering and loading assets, and the second semester task was to implement physics to the scene we had created in semester 1. [Click here for the repository with Physics.](https://github.com/AnthonySturdy/DX11-ShipScene-Physics)

<img src="https://github.com/AnthonySturdy/AnthonySturdy/blob/master/Ship_1.gif?raw=true">

### Features
<li>Hierarchical Scene Graph / Loading Scene from JSON file</li>
<li>Water Shader</li>
<li>Diffuse, Ambient and Specular Lighting</li>
<li>Specular and Normal mapping</li>
<li>Texturing</li>
<li>Ship Controller</li>
<li>Ship Bouyancy (shader visual effect)</li>
<li>First and Third person cameras</li>
<li>Skybox</li>
<li>Fog</li>

### What I learned
The development of this application introduced me to using a programmable pipeline for 3D rendering (had previously only used Fixed Function). It was my first experience using DirectX, which was useful as I had only used OpenGL previously. I learned to use HLSL for writing shaders, which also taught me about how lighting is applied to objects and how image effects such as fog are achieved. Lastly, this improved my C++ and general programming skills, and the original codebase was refactored throughout the development to keep the code clean and maintainable.
