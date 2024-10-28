
#!/bin/bash
#!/bin/bash

# Get the current directory name
current_dir=${PWD##*/}

# Check if the current directory is 'build'
if [ "$current_dir" == "build" ]; then
    # Go back one level
    cd ../
fi


# Navigate to the build directory
cd build

./TradingKLX config.json