<shader name="smokeparticle">
    <shaderprogram src="__unlit" />
    <raster>
        <fill>fill</fill>
        <cull>none</cull>
        <antialiasing>true</antialiasing>
    </raster>
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
    <depth enable="false" writable="false" />
    <stencil enable="false" readable="false" writable="false" />
</shader>