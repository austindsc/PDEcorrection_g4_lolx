import pandas as pd

# Read the file
df = pd.read_csv('sio2-fusedQuartz_uv_web.txt', sep='\t')

# Save the last column to a new file
df['k'].to_csv('new_file.txt', index=False, header=True)

# Remove the last column
df = df.drop('n', axis=1)

# Save the dataframe without the last column back to the original file
df.to_csv('sio2_KINDEX_fusedQuartz_uv_web.txt', index=False, sep='\t')