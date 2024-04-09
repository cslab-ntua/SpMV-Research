import numpy as np
from numpy import ma
import pandas as pd
from matplotlib import pyplot as plt
from matplotlib import scale as mscale
from matplotlib import transforms as mtransforms
from matplotlib.ticker import FixedLocator
import matplotlib.ticker

class SegmentedScale(mscale.ScaleBase):
    name = 'segmented'

    def __init__(self, axis, **kwargs):
        mscale.ScaleBase.__init__(self,axis)
        self.points = kwargs.get('points',[0,1])
        self.lb = self.points[0]
        self.ub = self.points[-1]

    def get_transform(self):
        return self.SegTrans(self.lb, self.ub, self.points)

    def set_default_locators_and_formatters(self, axis):
        axis.set_major_locator(FixedLocator(self.points))

    def limit_range_for_scale(self, vmin, vmax, minpos):
        return max(vmin, self.lb), min(vmax, self.ub)

    class SegTrans(mtransforms.Transform):
        input_dims = 1
        output_dims = 1
        is_separable = True

        def __init__(self, lb, ub, points):
            mtransforms.Transform.__init__(self)
            self.lb = lb
            self.ub = ub
            self.points = points

        def transform_non_affine(self, a):
            masked = a # ma.masked_where((a < self.lb) | (a > self.ub), a)
            return np.interp(masked, self.points, np.arange(len(self.points)))

        def inverted(self):
            return SegmentedScale.InvertedSegTrans(self.lb, self.ub, self.points)

    class InvertedSegTrans(SegTrans):

        def transform_non_affine(self, a):
            return np.interp(a, np.arange(len(self.points)), self.points)
        def inverted(self):
            return SegmentedScale.SegTrans(self.lb, self.ub, self.points)

# Now that the Scale class has been defined, it must be registered so
# that ``matplotlib`` can find it.
mscale.register_scale(SegmentedScale)

if __name__ == '__main__':
    font = {'family' : 'normal',
        'size'   : 16}
    matplotlib.rc('font', **font)
    df = pd.read_csv("~/sptrsv_aug_26.csv")
    names = df.Matrix.unique()
    for name in names:
        df_test = df[df["Matrix"] == name]
        df.loc[df["Matrix"] == name,'MKL Speedup'] = (df_test[["SpTRSV MKL Parallel Executor"]].min(axis=1) / df_test[["SpTRSV Vec2 Parallel"," SpTRSV DDT Parallel Executor"]].min(axis=1)).max()
        df.loc[df["Matrix"] == name,'Sympiler Speedup'] = (df_test["Supernodal Sympiler"] / df_test[["SpTRSV Vec2 Parallel"," SpTRSV DDT Parallel Executor"]].min(axis=1)).max()
    #df["CSR5 Speedup"] = df["SpMVCSR5 Parallel Executor"] / df[["SpMV DDT Parallel Executor"," SpMV Vec 1_4 Parallel"]].min(axis=1)
    fig, ( ax) = plt.subplots()
    ax.scatter(df["NNZ"],df["MKL Speedup"],color="black",label="MKL Baseline")
    ax.set_xscale('log')
    ax.set_yscale('segmented', points=np.array([0,1,1.25,1.5,1.75,2,4,5,15]))
    # ax.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(3))
    # ax.set_yticks([1,2,16])
    ax.set_xlabel("NNZ")
    ax.set_ylabel("Speedup")
    ax.scatter(df["NNZ"],df["Sympiler Speedup"],color="red",label="Sympiler Baseline")
    ax.legend(loc="upper left")
    plt.show()
