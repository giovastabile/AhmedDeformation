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

#include "SteadyNSSimple.H"
#include "ITHACAstream.H"
#include "IOmanip.H"
#include "RBFMotionSolver.H"

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
            // Create the RBF object
            ms = new RBFMotionSolver(mesh, *dyndict);
            // Catch the indices of the moving points.
            movingIDs = ms->movingIDs();
            // Save the starting position of the moving points.
            x0 = ms->movingPoints();
            curX = x0;
            // Save the starting position of the whole mesh.
            point0 = ms->curPoints();
        }

        volVectorField& U;
        List<vector> x0;
        vectorField point0;
        List<vector> curX;
        IOdictionary* dyndict;
        RBFMotionSolver* ms;
        labelList movingIDs;

        // This function moves linearly the coordinates of the points into points2Move 
        // where sMax is the displacement of the rear top corner of the ahmed body.
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
    // Create the set of angles you want to use to move the mesh.
    // Angles are evaluated with respect to the top horizontal part of the ahmed body.
    Eigen::VectorXd beta = Eigen::VectorXd::LinSpaced(50,15,35);
    // Create the box containing the point you want to move.
    Eigen::MatrixXd Box(2, 3);
    Box << -0.2012, -0.1945, 0.338,
        0.01, 0.1945, 0.05;
    // Create a list containing the indices of the patch you are interested in into the box
    List<label> movPat;
    movPat.append(6);

    scalarList x;

    for (int k = 0; k < beta.size(); k++)
    {
        std::cout << "Exporting mesh for sample: " << k+1 << std::endl;
        // Reset back the mesh to the starting configuration.
        mesh.movePoints(example.point0);
        List<vector> points2Move;
        labelList boxIndices = ITHACAutilities::getIndicesFromBox(example._mesh(), movPat, Box, points2Move);
        // Evaluate x-axis positions to catch extream ones.
        for (label i = 0; i < points2Move.size(); i++)
	    {
	        x.append(points2Move[i].component(0));
	    }
        double sMax = (max(x)-min(x)) * (std::tan(M_PI * 25 / 180) - std::tan(M_PI * beta(k) / 180));
        // Move the points into points2Move.
        example.linearMovePts(sMax, points2Move);
        // Set new position values using full mesh indices.
        ITHACAutilities::setIndices2Value(boxIndices, points2Move, example.movingIDs, example.curX);
        // Calculate displacements between new and default configurations.
        example.ms->setMotion(example.curX - example.x0);
        // Use displacements to move the mesh.
        mesh.movePoints(example.ms->curPoints());
        // Export the deformed mesh.
        ITHACAstream::writePoints(mesh.points(), "./ITHACAoutput/Offline/", name(k + 1) + "/polyMesh/");
    }
}
