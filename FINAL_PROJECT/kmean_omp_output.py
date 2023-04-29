import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from matplotlib.patches import Patch
from matplotlib.font_manager import FontProperties
import numpy as np
import pandas as pd
import rasterio
from sklearn.cluster import KMeans

# arr = np.array([[97.602743, 30.324161],
#                 [97.60284224, 30.32364238],
#                 [97.60230312, 30.32364238],
#                 [97.602394, 30.324453],
#                 [97.603051, 30.324997]])

# x_gcs_tarp = arr[:, 0]
# y_gcs_tarp = arr[:, 1]


arr = np.array([[97.6027, 30.3242],
                [97.6028, 30.3236],
                [97.6023, 30.3236],
                [97.6024, 30.3245],
                [97.6031, 30.325]])

x_gcs_tarp = arr[:, 0]
y_gcs_tarp = arr[:, 1]

# Open raster file with rasterio
rst = rasterio.open('ARCAClippedGCS2.tif')

# Read raster data into numpy array
data = rst.read(1)

# Define colormap for raster
cmap = LinearSegmentedColormap.from_list('mycmap', [(0, 'black'), (1, 'white')])

# Create figure and axis objects
fig, ax = plt.subplots(figsize=(10, 10))

# Plot raster as image
ax.imshow(data, cmap=cmap, extent=[rst.bounds.left, rst.bounds.right, rst.bounds.bottom, rst.bounds.top])

# Set bounds
x_gcs_left = 97.605117453000 #the longitude is made positive
y_gcs_top = 30.326369828000
length_gcs = 0.003757268 #length of both sides of raster in gcs

x_length = rst.bounds.right-rst.bounds.left #length of raster in x in matplotlib
y_length = rst.bounds.top-rst.bounds.bottom #length of raster in y in matplotlib


# # gcs points for empty tarps
# x_gcs_tarp = np.array([97.602743, 97.60284224, 97.60230312, 97.602394, 97.603051])
# y_gcs_tarp = np.array([30.324161, 30.32364238, 30.32364238, 30.324453, 30.324997])



def convert_gcs_to_matplotlib(color, label, x_gcs, y_gcs, length_gcs, x_length, y_length):
    x_points = ((x_gcs_left - x_gcs)/length_gcs) * x_length
    y_points = ((y_gcs_top - y_gcs)/length_gcs) * y_length
    x = rst.bounds.left + x_points
    y = rst.bounds.top - y_points
    for i in range(len(x)):
        plt.annotate(labels[i], (x[i], y[i]),  # Label and position
                      textcoords="offset points",  # Specify label position relative to data points
                      xytext=(10,-2),  # Offset of label from data point in points
                      color='white',  # Set custom color
                      fontname='Arial',
                      fontsize=13,
                      ha='center',)  # Horizontal alignment of label text
    ax.scatter(x, y, color=color, marker='s', label=label)
    
    
labels = list(range(1, len(x_gcs_tarp)+1))
convert_gcs_to_matplotlib('yellow', 'Empty Tarp', x_gcs_tarp, y_gcs_tarp, length_gcs, x_length, y_length)
tarps = np.array([[labels[i], round(y_gcs_tarp[i],7), -round(x_gcs_tarp[i],7)] for i in range(0, len(labels))])

header = np.array(['Target', 'Latitude', 'Longitude'])
data = np.vstack((header, tarps))

# Show the plot
plt.title('Map of Clustered Coordinates', fontname='Arial', fontsize=28)
legend_font = FontProperties(family='Arial', size=16, weight='bold')
#plt.legend(prop=legend_font,facecolor='grey')
plt.xticks([])
plt.yticks([])
plt.xlabel('')
plt.ylabel('')
table = ax.table(cellText=data, colLabels=None, cellColours=[['gray'] * len(header)] + [['white'] * len(header)] * (len(data) - 1), cellLoc='center', loc='bottom')
table.auto_set_font_size(False)
table.set_fontsize(10)
table.scale(1, 1.5)
ax.axis('off')
plt.show()











