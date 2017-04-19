import numpy as np
import matplotlib.pyplot as pt
import matplotlib.cm


def imshow(image, channel=None):
    if channel is None:
        im = image
    else:
        im = image[:, :, channel]
    im[image[:, :, 3] == 0] = np.nan

    pt.figure(figsize=(12, 12), dpi=72)

    c = matplotlib.cm.viridis
    c.set_bad('black', alpha=None)
    ax = pt.imshow(im, interpolation='none', cmap=c)

    pt.colorbar(ax, fraction=0.046, pad=0.04)
    pt.tight_layout()
    pt.show()
