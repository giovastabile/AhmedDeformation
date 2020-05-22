# Ahmed Body Deformation Test case

Qua metterei una descrizione del problema. Tipo. We show a mesh morphing technique based on RBF on an Ahmed body Geometry. The implmentation is based on OpenFOAM and ITHACA-FV.

## 0. Description of ITHACA-FV
**ITHACA-FV** is an implementation in **OpenFOAM** of several reduced order modelling techniques. **ITHACA-FV** is designed for [**OpenFOAM 6.0**](https://openfoam.org/version/6), [**OpenFOAM 5.0**](https://openfoam.org/version/5-0), [**OpenFOAM v1812**](https://www.openfoam.com/releases/openfoam-v1812/) and [**OpenFOAM 1906**](https://www.openfoam.com/releases/openfoam-v1906/) but it can be easily adapted also to other versions of OpenFOAM. 

**ITHACA-FV** can also be used as a basis for more advanced projects that would like to assess the capability of reduced order models in their existing **OpenFOAM**-based software, thanks to the availability of several reduced order methods and algorithms.

Linear and non-linear algebra operations which are not already implemented in OpenFOAM are performed with the external library [**Eigen**](http://eigen.tuxfamily.org/index.php?title=Main_Page). The source code of Eigen 3.3.7 is provided together with ITHACA-FV and is located in the [src/thirdyparty/Eigen](./src/thirdparty/Eigen) folder.  For the EigenValue decomposition it is also possible to rely on the [**Spectra-0.7.0**](https://spectralib.org/) library and the source code is provided in the [src/thirdyparty/spectra](./src//thirdparty/spectra) folder.

**ITHACA-FV** has been tested on ubuntu 16.04, CentOS 7, ArchLinux but can be easily compiled on any linux distribution with a compiled version of OpenFOAM 6.0, OpenFOAM 5.0 or OpenFOAM 1812.

## 1. Prerequisites
**Installation** of one of the following versions of OpenFOAM:
* [**OpenFOAM 5.0**](https://openfoam.org/version/5-0)
* [**OpenFOAM 6.0**](https://openfoam.org/version/6) 
* [**OpenFOAM 1812**](https://www.openfoam.com/releases/openfoam-v1812/)
* [**OpenFOAM 1906**](https://www.openfoam.com/releases/openfoam-v1906/) 

**Installation** of the [**ITHACA-FV**](https://github.com/mathLab/ITHACA-FV) library. Instructions can be found at the GitHub webpage of the library.

## 2. Usage of the test case

### 1. Clone the folder [**AhmedDeformation**](https://github.com/giovastabile/AhmedDeformation):
```
git clone git@github.com:giovastabile/AhmedDeformation.git
```
### 2. Open a terminal into the folder;
### 3. Source both **OPENfoam** and the library **ITHACA-FV** depending on the place where you cloned the folder and on the version of OpenFOAM you are using:
```
source /opt/openfoam6/etc/bashrc
source /home/ITHACA-FV/etc/bashrc
```
### 4. Compile the test case by the use of the command:
```
wmake
```
### 4. Run the test case by the use of the command:
```
testAhmed
```
More details about the way the code works are reported into che file `testAhmed.C`. All the deformed geometries are automatically exported into the folder `ITHACAoutput/Offline` and they can be shown by the use of **ParaView**.

## 3. Authors and contributors

qua metterei me e te e non la farei uguale ad ITHACA

**ITHACA-FV** is currently developed and mantained at [SISSA mathLab](http://mathlab.sissa.it/) by [Dr. Giovanni Stabile](mailto:gstabile@sissa.it) under the supervision of [Prof. Gianluigi Rozza](mailto:gianluigi.rozza@sissa.it)

Contact us by email for further information or questions about **ITHACA-FV**, or open an ''Issue'' on this website. **ITHACA-FV** is at an early development stage, so contributions improving either the code or the documentation are welcome, both as patches or merge requests on this website. If you are willing to contribute please follow the [developer instructions](https://github.com/mathLab/ITHACA-FV/tree/master/src).

## 4. How to cite

qua metterei il nostro paper e lo farei anche qui specifico

Most of the theoretical aspects behind **ITHACA-FV** are deeply explained in [<b> Stabile2017CAIM </b>](https://arxiv.org/pdf/1701.03424.pdf) and [<b> Stabile2017CAF </b>](https://arxiv.org/pdf/1710.11580.pdf).
For this reason, if you use this software, please consider citing the mentioned works, reported in the following bibtex entries:
```
@Article{Stabile2017CAIM,
Title                    = {{POD-Galerkin reduced order methods for CFD using Finite Volume Discretisation: vortex shedding around a circular cylinder}},
Author                   = {Stabile, Giovanni and Hijazi, Saddam and Mola, Andrea and Lorenzi, Stefano and Rozza, Gianluigi},
Journal                  = {Communications in Applied and Industrial Mathematics},
Year                     = {(2017)},
Volume                   = {8},
Number                   = {1},
pages                    = {210-236},
Doi                      = {10.1515/caim-2017-0011}}
```

```
@Article{Stabile2017CAF,
Title                    = {{Finite volume POD-Galerkin stabilised reduced order methods for the parametrised incompressible Navier-Stokes equations}},
Author                   = {Stabile, Giovanni and Rozza, Gianluigi},
Journal                  = {Computers & Fluids},
Year                     = {2018},
Doi                      = {10.1016/j.compfluid.2018.01.035}}
```


and cite the [ITHACA-FV website](http://mathlab.sissa.it/ITHACA-FV).


## 5. License
**ITHACA-FV** is freely available under the GNU LGPL, version 3.
