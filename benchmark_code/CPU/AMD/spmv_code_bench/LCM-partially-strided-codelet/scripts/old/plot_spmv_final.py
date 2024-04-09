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
    df = pd.read_csv("final_spmv_speedups.csv")
    names = df.Matrix.unique()
    for name in names:
        df_test = df[df["Matrix"] == name]
        df.loc[df["Matrix"] == name,'MKL Speedup'] = (df_test[[" SpMV MKL Parallel Executor"," SpMV Parallel Base"]].min(axis=1) / df_test[[" SpMV Vec 1_4 Parallel","SpMV DDT Parallel Executor"]].min(axis=1)).max()
        df.loc[df["Matrix"] == name,'CSR5 Speedup'] = (df_test[["SpMVCSR5 Parallel Executor"]].min(axis=1) / df_test[[" SpMV Vec 1_4 Parallel","SpMV DDT Parallel Executor"]].min(axis=1)).max()
    fig, (ax,ax1) = plt.subplots(1,2)
    # df = df[df["CSR5 Speedup"].notnull()]
    df = df[( df[" size_cutoff"] == 1 ) & ( df[" col_threshold"] == 1 )]
    ax.scatter(df["NNZ"],df["MKL Speedup"],color="black")
    ax.set_xscale('log')
    ax.set_yscale('segmented', points=np.array([0.9,1,1.25,1.5,2,10,20]))
    # ax.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(3))
    # ax.set_yticks([1,2,16])
    ax.set_xlabel("NNZ")
    ax.set_ylabel("Speedup")
    trans = mtransforms.ScaledTranslation(-20/72, 7/72, fig.dpi_scale_trans)
    ax.text(0.0, 1.0, "a)", transform=ax.transAxes + trans,
            fontsize='medium', va='bottom',fontweight="bold")

    ax.axhline(y=1.0,color='r',linestyle='-')
    ax1.scatter(df["NNZ"],df["CSR5 Speedup"],color="black")
    ax1.set_xscale('log')
    ax1.set_yscale('segmented', points=np.array([0.2,0.5,1,1.25,1.5,2,4,5]))
    # ax.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(3))
    # ax.set_yticks([1,2,16])
    ax1.set_xlabel("NNZ")
    ax1.set_ylabel("Speedup")
    ax1.text(0.0, 1.0, "b)", transform=ax1.transAxes + trans,
            fontsize='medium', va='bottom',fontweight="bold")
    ax1.axhline(y=1.0,color='r',linestyle='-')
    plt.show()
