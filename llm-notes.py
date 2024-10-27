import os

# List of files to merge
files_to_merge = ['Trade.h','DataProcessor.cpp', 'DataProcessor.h', 'DataProcessor.h', 'PerformanceAssessor.cpp', 'PerformanceAssessor.h','PriceData.h','SlidingWindow.h','TechnicalIndicators.cpp','TechnicalIndicators.h','TradingKLX.cpp','TradingKLX.h', 'TradingSignal.h','TradingStrategy.cpp','TradingStrategy.h''Utilities.cpp','Utilities.h','Trade.h','TradeData.h']
# files_to_merge = ['Trade.h','BacktestManager.cpp','BacktestManager.h','OandA_API.cpp','OandA_API.hpp','Dockerfile','docker-compose.yml','CMakeLists.txt', 'DataProcessor.cpp', 'DataProcessor.h', 'DataProcessor.h', 'PerformanceAssessor.cpp', 'PerformanceAssessor.h','PriceData.h','SlidingWindow.h','TechnicalIndicators.cpp','TechnicalIndicators.h','TradingKLX.cpp','TradingKLX.h', 'TradingSignal.h','TradingStrategy.cpp','TradingStrategy.h','Utilities.cpp','Utilities.h']

# Output file name
output_file = 'merged_output.txt'

# Open the output file in write mode
with open(output_file, 'w') as outfile:
    # Iterate through each file in the list
    for filename in files_to_merge:
        # Check if the file exists
        if os.path.exists(filename):
            # Write the filename followed by a colon and newline
            outfile.write(f"{filename}:\n")

            # Open and read the contents of the current file
            with open(filename, 'r') as infile:
                # Write the contents of the file
                outfile.write(infile.read())

            # Add an extra newline for separation
            outfile.write('\n\n')
        else:
            print(f"Warning: File {filename} not found. Skipping.")

print(f"Merged contents have been written to {output_file}")