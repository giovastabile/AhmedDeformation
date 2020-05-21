/*---------------------------------------------------------------------------*\
     ██╗████████╗██╗  ██╗ █████╗  ██████╗ █████╗       ███████╗██╗   ██╗
     ██║╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗      ██╔════╝██║   ██║
     ██║   ██║   ███████║███████║██║     ███████║█████╗█████╗  ██║   ██║
     ██║   ██║   ██╔══██║██╔══██║██║     ██╔══██║╚════╝██╔══╝  ╚██╗ ██╔╝
     ██║   ██║   ██║  ██║██║  ██║╚██████╗██║  ██║      ██║      ╚████╔╝
     ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝      ╚═╝       ╚═══╝

 * In real Time Highly Advanced Computational Applications for Finite Volumes
 * Copyright (C) 2017 by the ITHACA-FV authors
-------------------------------------------------------------------------------

  License
  This file is part of ITHACA-FV

  ITHACA-FV is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ITHACA-FV is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ITHACA-FV. If not, see <http://www.gnu.org/licenses/>.
\*---------------------------------------------------------------------------*/

#include<iostream>
#include "argList.H"
#include "Time.H"
#include "fvMesh.H"
#include "RBFMotionSolver.H"
#include "dictionary.H"
#include "fvCFD.H"
#include "IOmanip.H"
#include "SteadyNSSimple.H"
#include "ITHACAutilities.H"
#include <Eigen/Dense>
#include "EigenFunctions.H"
#define _USE_MATH_DEFINES
#include <cmath>
#include "pointPatchField.H"

class geomParVW : public SteadyNSSimple
{
    public:
        explicit geomParVW(int argc, char* argv[])
            :
            SteadyNSSimple(argc, argv),
            U(_U())
        {
            fvMesh& mesh = _mesh();
            IOdictionary* dyndict = new IOdictionary
            (
                IOobject
                (
                    "dynamicMeshDictRBF",
                    "./constant",
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE
                )
            );
            ms = new RBFMotionSolver(mesh, *dyndict);
            vectorField motion(ms->movingPoints().size(), vector::zero);
            movingIDs = ms->movingIDs();
            x0 = ms->movingPoints();
            curX = x0;
            point0 = ms->curPoints();
        }

        volVectorField& U;
        List<vector> x0;
        vectorField point0;
        List<vector> curX;
        IOdictionary* dyndict;
        RBFMotionSolver* ms;
        labelList movingIDs;

        labelList getIndicesFromBox(fvMesh& mesh, label ind, Eigen::MatrixXd Box,
                                    List<vector>& points2Move)
        {
            points2Move.resize(0);
            pointField meshPoints(mesh.points());
            const polyPatch& patchFound = mesh.boundaryMesh()[ind];
            labelList labelPatchFound(patchFound.meshPoints());
            labelList boxIndices;

            for (int i = 0; i < labelPatchFound.size(); i++)
            {
                auto px = meshPoints[labelPatchFound[i]].component(0);
                auto py = meshPoints[labelPatchFound[i]].component(1);
                auto pz = meshPoints[labelPatchFound[i]].component(2);

                if (px >= min(Box(0, 0), Box(1, 0)) && py >= min(Box(0, 1), Box(1, 1)) &&
                        pz >= min(Box(0, 2), Box(1, 2)) && px <= max(Box(0, 0), Box(1, 0))
                        && py <= max(Box(0, 1), Box(1, 1)) && pz <= max(Box(0, 2), Box(1, 2)) )
                {
                    boxIndices.append(labelPatchFound[i]);
                    points2Move.append(meshPoints[labelPatchFound[i]]);
                }
            }

            return boxIndices;
        }

        void linearMovePts(double sMax, List<vector>& points2Move)
        {
            scalarList x;
            scalarList z;

            for (label i = 0; i < points2Move.size(); i++)
            {
                x.append(points2Move[i].component(0));
                z.append(points2Move[i].component(2));
            }

            double xMin = min(x);
            double xMax = max(x);
            double zMin = min(z);
            double zMax = max(z);

            for (label i = 0; i < points2Move.size(); i++)
            {
                points2Move[i].component(2) = points2Move[i].component(2) +
                                              (points2Move[i].component(0) - xMin) / (xMax - xMin) *
                                              (points2Move[i].component(2) - zMin) / (zMax - (zMax - 0.244179) /
                                                      (xMax - xMin) * (points2Move[i].component(0) - xMin)) * sMax;
            }
        }
};

int main(int argc, char* argv[])
{
    geomParVW example(argc, argv);
    fvMesh& mesh = example._mesh();
    Eigen::VectorXd beta = Eigen::VectorXd::LinSpaced(50,15,35);
    Eigen::MatrixXd Box(2, 3);
    Box << -0.2012, -0.1945, 0.338,
        0.01, 0.1945, 0.05;

    for (int k = 0; k < beta.size(); k++)
    {
        std::cout << "Exporting mesh for sample: " << k+1 << std::endl;
        mkDir("./defGeom/" + name(k + 1));
        system("cp -r constant defGeom/" + name(k + 1) + "/constant");
        system("cp -r system defGeom/" + name(k + 1) + "/system");
        system("cp -r 0 defGeom/" + name(k + 1) + "/0");
        mesh.movePoints(example.point0);
        List<vector> points2Move;
        labelList boxIndices = example.getIndicesFromBox(mesh, 6, Box, points2Move);
        double sMax = 0.2012 * (std::tan(M_PI * 25 / 180) - std::tan(M_PI * beta(k) / 180));
        example.linearMovePts(sMax, points2Move);
        ITHACAutilities::setIndices2Value(boxIndices, points2Move, example.movingIDs,
                                          example.curX);
        example.ms->setMotion(example.curX - example.x0);
        mesh.movePoints(example.ms->curPoints());
        ITHACAstream::writePoints(mesh.points(), "./ITHACAoutput/Offline/",
                                  name(k + 1) + "/polyMesh/");
        ITHACAstream::writePoints(mesh.points(), "defGeom", name(k + 1) + "/constant/polyMesh/");

    }
}