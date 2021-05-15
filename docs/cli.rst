.. include:: global.rst

Command line interface
======================

Using the command line you can issue one single command and let WorldEngine generate a world for you.

   worldengine [<operation> [<file>]] [<options>] [world|plates|ancient_map|info]

Arguments
~~~~~~~~~

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Argument
     - Description
   * - operation
     - Valid operations: world, plates, ancient_map, info, export
   * - file
     - Input filename for info and export

Generic options
~~~~~~~~~~~~~~~

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Short
     - Long
     - Description
   * -
     - --version
     - Print version string
   * - -h
     - --help
     - Produce help message
   * - -v
     - --verbose
     - Enable verbose messages

Configuration
~~~~~~~~~~~~~

.. list-table::
   :widths: auto
   :header-rows: 1
   :class: rst-valign-top

   * - Short
     - Long
     - Description
   * - -o <dir>
     - --output-dir <dir>
     - Set output directory |br|
       *Default = .*
   * - -n <arg>
     - --worldname <arg>
     - Set world name
   * -
     - --format <arg>
     - Set file format |br|
       Valid formats: hdf5, protobuf |br|
       *Default = protobuf*
   * - -s <arg>
     - --seed <arg>
     - Initializes the pseudo-random generation
   * - -t <arg>
     - --step <arg>
     - Specifies how far to proceed in the world generation process |br|
       Valid steps: plates, precipitations, full |br|
       *Default = full*
   * - -x <arg>
     - --width <arg>
     - Width of the world to be generated |br|
       *Default = 512*
   * - -y <arg>
     - --height <arg>
     - Height of the world to be generated |br|
       *Default = 512*
   * - -q <arg>
     - --plates <arg>
     - Number of plates |br|
       Valid values: [1, 100] |br|
       *Default = 10*
   * -
     - --black-and-white
     - Generate maps in black and white

Generate options
~~~~~~~~~~~~~~~~

The generate options are for the plate and world modes only.

.. list-table::
   :widths: auto
   :header-rows: 1
   :class: rst-valign-top

   * - Short
     - Long
     - Description
   * - -r
     - --rivers
     - Generate rivers map
   * -
     - --grayscale-heightmap
     - Produce a grayscale heightmap
   * -
     - --ocean-level <arg>
     - Elevation cutoff for sea level |br|
       *Default = 1*
   * -
     - --temps <arg>
     - Provide alternate ranges for temperatures |br|
       *Default = 0.126 0.235 0.406 0.561 0.634 0.876*
   * -
     - --humidity <arg>
     - Provide alternate ranges for humidities |br|
       *Default = 0.059 0.222 0.493 0.764 0.927 0.986 0.998*
   * -
     - --gamma-value <arg>
     - Gamma value for temperature/precipitation gamma correction curve |br|
       Valid values: Positive floating point |br|
       *Default = 1.25*
   * -
     - --gamma-offset <arg>
     - Adjustment value for temperature/precipitation gamma correction curve |br|
       Valid values: [0.0, 1.0) |br|
       *Default = 0.2*
   * -
     - --not-fade-borders
     - Don't fade borders
   * -
     - --scatter
     - Generate scatter plot
   * -
     - --sat
     - Generate satellite map
   * -
     - --ice
     - Generate ice caps map
   * -
     - --world-map
     - Generate world map
   * -
     - --elevation-map
     - Generate elevation map
   * -
     - --elevation-shadows
     - Draw shadows on elevation map

Ancient map options
~~~~~~~~~~~~~~~~~~~

The ancient map options are for the ancient map mode only.

.. list-table::
   :widths: auto
   :header-rows: 1
   :class: rst-valign-top

   * - Short
     - Long
     - Description
   * - -w <filename>
     - --worldfile <filename>
     - File to be loaded
   * - -g <filename>
     - --generated-file <filename>
     - File to be generated
   * - -f <arg>
     - --resize-factor <arg>
     - Resize factor |br|
       NOTE: This can only be used to increase the size of the map |br|
       *Default = 1*
   * -
     - --sea-color <arg>
     - Sea color |br|
       Valid values: blue, brown |br|
       *Default = brown*
   * -
     - --not-draw-biome
     - Don't draw biome
   * -
     - --not-draw-mountains
     - Don't draw mountains
   * -
     - --not-draw-rivers
     - Don't draw rivers
   * -
     - --draw-outer-border
     - Draw outer land border
     
Export options
~~~~~~~~~~~~~~

.. list-table::
   :widths: auto
   :header-rows: 1
   :class: rst-valign-top

   * - Long
     - Description
   * - --export-format <arg>
     - Export to a specific format |br|
       All possible formats: https://www.gdal.org/formats_list.html |br|
       *Default = PNG*
   * - --export-datatype <arg>
     - Type of stored data |br|
       Valid values: int16, int32, uint8, uint16, uint32, float32, float64 |br|
       *Default = uint16*
   * - --export-dimensions <arg>
     - Export to desired dimensions |br|
       *Example: 4096 4096*
   * - --export-normalize <arg>
     - Normalize the data set between min and max |br|
       *Example: 0 255*
   * - --export-subset <arg>
     - Selects a subwindow from the data set |br|
       Arguments: <xoff> <yoff> <xsize> <ysize> |br|
       *Example: 128 128 256 256*
