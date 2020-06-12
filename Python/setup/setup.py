from distutils.core import setup, Extension

files_path = '../../Simulation/Simulation/'

setup(name='EnvManagerPkg',
      version='1.0',
      ext_modules=[
          Extension('EnvManager',
                    sources=['EnvManagerWrapper.cpp',
                             files_path+'EnvManager.cpp',
                             files_path+'Simulation.cpp',
                             files_path+'Distances.cpp',
                             files_path+'PredatorSwarm.cpp',
                             files_path+'PreySwarm.cpp',
                             files_path+'LSTM.cpp',
                             files_path+'RNN.cpp',
                             files_path+'json11.cpp'],
                    include_dirs=['../../Simulation/packages/Eigen.3.3.3/build/native/include/'],
                    # libraries=["sfml-sysem-s-d","sfml-grapics-s-d"]
                    )]
      )
