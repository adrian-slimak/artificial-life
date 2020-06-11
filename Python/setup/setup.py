from distutils.core import setup, Extension

setup(name='EnvManagerPkg',
      version='1.0',
      ext_modules=[
          Extension('EnvManager',
                    sources=['EnvManagerWrapper.cpp',
                             '../Simulation/EnvManager.cpp',
                             '../Simulation/Simulation.cpp',
                             '../Simulation/Distances.cpp',
                             '../Simulation/PredatorSwarm.cpp',
                             '../Simulation/PreySwarm.cpp',
                             '../Simulation/LSTM.cpp',
                             '../Simulation/RNN.cpp'],
                    include_dirs=['../packages/Eigen.3.3.3/build/native/include/'],
                    # libraries=["sfml-sysem-s-d","sfml-grapics-s-d"]
                    )]
      )
