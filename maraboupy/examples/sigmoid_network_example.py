import sys
from pathlib import PosixPath

marabou_path = PosixPath(r"/cs/labs/guykatz/yoni_mantzur/marabou")

sys.path.append(str(marabou_path))

import os

# marabou_path = Path(r'/mnt/c/Users/t-yomant/lab/Marabou')

path_to_sigmoids = marabou_path / r'resources/tf/frozen_graph/sigmoids/'
path_to_experiments = marabou_path / r'maraboupy/examples/networks/sigmoids'

from maraboupy import MarabouCore, MarabouNetwork, Marabou, MarabouUtils


import numpy as np

try:
    exp_num = max(map(int, map(lambda y: ''.join(y),
                               map(lambda f: filter(lambda c: c.isdigit(), f), filter(lambda dir: 'experiment_' in dir,
                                                                                      os.listdir(
                                                                                          path_to_experiments)))))) + 1
except ValueError:
    exp_num = 1

exp_dir = path_to_experiments / f'experiment_{exp_num}'
if not os.path.exists(exp_dir):
    os.mkdir(exp_dir)

large = 100.0
deltas = [0.01, 0.015, 0.023, 0.029, 0.03, 0.09, 0.1, 0.3, 0.4, 0.6, 0.7]

org_exp_dir = exp_dir
for name in range(10, 100, 10):
    exp_dir = org_exp_dir / f'{name}'

    os.mkdir(exp_dir)


    x = [0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0.32941177, 0.7254902, 0.62352943,
         0.5921569, 0.23529412, 0.14117648, 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0.87058824, 0.99607843, 0.99607843, 0.99607843, 0.99607843,
         0.94509804, 0.7764706, 0.7764706, 0.7764706, 0.7764706,
         0.7764706, 0.7764706, 0.7764706, 0.7764706, 0.6666667,
         0.20392157, 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.2627451, 0.44705883,
         0.28235295, 0.44705883, 0.6392157, 0.8901961, 0.99607843,
         0.88235295, 0.99607843, 0.99607843, 0.99607843, 0.98039216,
         0.8980392, 0.99607843, 0.99607843, 0.54901963, 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0.06666667, 0.25882354, 0.05490196, 0.2627451,
         0.2627451, 0.2627451, 0.23137255, 0.08235294, 0.9254902,
         0.99607843, 0.41568628, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0.3254902, 0.99215686, 0.81960785, 0.07058824,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.08627451, 0.9137255,
         1., 0.3254902, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0.5058824, 0.99607843, 0.93333334, 0.17254902,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.23137255, 0.9764706,
         0.99607843, 0.24313726, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0.52156866, 0.99607843, 0.73333335, 0.01960784,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.03529412, 0.8039216,
         0.972549, 0.22745098, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0.49411765, 0.99607843, 0.7137255, 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.29411766, 0.9843137,
         0.9411765, 0.22352941, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0.07450981, 0.8666667, 0.99607843, 0.6509804, 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0.01176471, 0.79607844, 0.99607843,
         0.85882354, 0.13725491, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0.14901961, 0.99607843, 0.99607843, 0.3019608, 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0.12156863, 0.8784314, 0.99607843,
         0.4509804, 0.00392157, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0.52156866, 0.99607843, 0.99607843, 0.20392157, 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0.23921569, 0.9490196, 0.99607843,
         0.99607843, 0.20392157, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0.4745098, 0.99607843, 0.99607843, 0.85882354, 0.15686275,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.4745098, 0.99607843,
         0.8117647, 0.07058824, 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0., 0.,
         0., 0., 0., 0.]

    y = np.array([[-4.13783711, -5.86701435, -1.25894247, -0.45177739, -1.38287319,
                   -3.77124648, -7.53252938, 6.1390369, -7.75015215, 0.80588465]])

    with open(exp_dir / f'res_{name}.txt', "x") as res_file:

        with open(exp_dir / f'times_{name}.txt', "x") as times_file:

            for delta in deltas:
                print(delta, name)
                filename = path_to_sigmoids / f'mnist_{name}.pb'

                network = Marabou.read_tf(str(filename))  # type: MarabouNetwork.MarabouNetwork


                # Get the input and output variable numbers; [0] since first dimension is batch size
                inputVars = network.inputVars[0][0]
                outputVars = network.outputVars[0]

                # Set output bounds
                for var in outputVars:
                    network.setLowerBound(var, -large)
                    network.setUpperBound(var, large)

                # # Set input bounds
                for var in inputVars:
                    network.setLowerBound(var, x[var] - delta)
                    network.setUpperBound(var, x[var] + delta)

                new_var = network.getNewVariable()
                network.setLowerBound(new_var, 0)

                equation1 = MarabouUtils.Equation(MarabouCore.Equation.EquationType.EQ)
                equation1.addAddend(1, outputVars[7])
                equation1.addAddend(-1, outputVars[9])
                equation1.addAddend(1, new_var)
                equation1.setScalar(0)

                network.addEquation(equation1)

                network.outputVars = np.array([[new_var]])

                # network.evaluateWithMarabou(np.array([x]))
                # # Call to C++ Marabou solver
                # options = Marabou.createOptions(dnc=True, numWorkers=6, initialDivides=2, verbosity=0)
                vals, stats = network.solve(str(exp_dir / f'marabou_mnist_{name}_{delta}.log'))
                res_file.write(f'== Net with {name} sigmoids and delta: {delta}\n')
                res_file.write(f'number splits: {stats.getNumSplits()}\n')
                res_file.write(f'number active: {stats.getNumActivePlConstraints()} / {stats.getNumPlConstraints()}'
                               f'\n')
                res_file.write(f'number equations: {stats.getNumAbstractedEquations()}\n')
                res_file.write("result is: \n")

                if len(vals) == 0:
                    res_file.write("UNSAT\n")
                else:
                    res_file.write("SAT\n")
                    for j in range(len(network.inputVars)):
                        for i in range(network.inputVars[j].size):
                            res_file.write("input {} = {}\n".format(i, vals[network.inputVars[j].item(i)]))

                        for i in range(network.outputVars.size):
                            res_file.write("output {} = {}\n".format(i, vals[network.outputVars.item(i)]))

                times_file.write(f'sigmoids={name}\n')
                times_file.write(f'delta={delta}\n')
                times_file.write(f'result: {"sat" if vals else "unsat"}\n')
                times_file.write(f'time={(stats.getTotalTime() / 1000)}\n')

                res_file.flush()
                times_file.flush()
