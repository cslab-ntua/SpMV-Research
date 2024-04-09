import numpy as np

class codeket_model:
    def __init__(self):
        self.f_a = []
        self.f_b = []
        self.f_init = []
        self.g_a = []
        self.g_b = []
        self.g_init = []
        self.h_a = []
        self.h_b = []
        self.h_init = []
        self.i0 = []
        self.i1 = []


def compute_FOPD(ff, dim):
    di0 = []
    di1 = []
    if dim == 2:
        i0_len = len(ff)
        for i0 in range(i0_len-1):
            i1_len = len(ff[i0])
            i1p1_len = len(ff[i0+1])
            min_both = min(i1_len, i1p1_len)
            tmp = np.zeros(min_both-1 if min_both>0 else 0, dtype=int)
            for i1 in range(min_both-1):
                tmp[i1] = ff[i0 + 1][i1] - ff[i0][i1]
            di0.append(tmp)
            tmp = np.zeros(i1_len-1 if i1_len>0 else 0, dtype=int)
            for i1 in range(i1_len-1):
                tmp[i1] = ff[i0][i1+1] - ff[i0][i1]
            di1.append(tmp)
    return di0, di1


def opno_to_access_func(grp, op2i0, op2i1, f, g, h):
    ff = []
    gg = []
    hh = []
    tmp_f = []
    tmp_g = []
    tmp_h = []
    first_i0 = op2i0[grp[0]]
    for n in grp:
        if op2i0[n] == first_i0:
            tmp_f.append(f[op2i0[n]][op2i1[n]])
            tmp_g.append(g[op2i0[n]][op2i1[n]])
            tmp_h.append(h[op2i0[n]][op2i1[n]])
        else:
            ff.append(tmp_f)
            gg.append(tmp_g)
            hh.append(tmp_h)
            tmp_f = []
            tmp_g = []
            tmp_h = []
            first_i0 = op2i0[grp[n]]
    return ff, gg, hh


def get_bounds(dfdi0, dfdi1):
    cm = codeket_model()
    if dfdi0.all(element == dfdi0[0] for element in dfdi0):
        cm.f_a = dfdi0[0]

    if dfdi1.all(element == dfdi1[0] for element in dfdi1):
        cm.f_b = dfdi1[0]


def mine_for_PSC(f, g, h):
    dfdi0, dfdi1 = compute_FOPD(f, 2)
    dgdi0, dgdi1 = compute_FOPD(g, 2)
    dhdi0, dhdi1 = compute_FOPD(h, 2)
    for i in range(0, len(f), 2):
        if dfdi0[i].all(element == dfdi0[i][0] for element in dfdi0[i]) and dfdi0[i].all(element == dfdi0[i][0] for element in dfdi0[i]):
            print("f")



class codelet:
    def __init__(self, i0, i1, f, g, h):
        # f, g, and h are large functions
        self.i0 = i0
        self.i1 = i1
        self.f = f
        self.g = g
        self.h = h
        self.dfdi0 = []
        self.dfdi1 = []
        self.dgdi0 = []
        self.dgdi1 = []
        self.dhdi0 = []
        self.dhdi1 = []
        self.type = 1
        self.variable_space = 0
        self.num_strided = 0

    def codelet_type(self, f, g, h):
        # Checking all space to fit it into one type.
        dfdi0, dfdi1 = compute_FOPD(f, 2)
        dgdi0, dgdi1 = compute_FOPD(g, 2)
        dhdi0, dhdi1 = compute_FOPD(h, 2)
        # first check the iterations space is equal or not
        for i in range(len(f)):
            if len(f[i]) != len(g[i]) != len(h[i]):
                # check whether dfdi0 is strided
                variable_space = 1  # PSC I
        for i in range(0, len(f), 2):
            if dfdi0[i].all(element == dfdi0[i][0] for element in dfdi0[i]) and dfdi0[i].all(
                    element == dfdi0[i][0] for element in dfdi0[i]):
                print("f")

        for i in self.i0:
            if self.dfdi0[i][:] == self.dfdi0[0][0] or self.dfdi1[i][:] == self.dfdi1[0][0]: # or g or h
                # check whether dfdi0 is strided
                self.num_strided += 1  # PSC I
        # then check FOPDs on f

    def get_combination(self):
        s = 1

