import re

# Replace these multi-line strings with the full outputs from your terminal and TSV file.
terminal_output = """
G4WT5 > --> Event 0 starts with initial seeds (59960532,51374712).
G4WT5 > [1] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.36511e-08, 1.01882e-06)
G4WT7 > [1] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.99119e-08, 9.62914e-07)
G4WT6 > [1] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.04351e-06)
G4WT7 > [2] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (7.5e-08, 1.00494e-06)
G4WT1 > [1] Ionization of mu-α by cone 29 (x,y)=(6.75e-07, 0) at (rho,z) = (4.06346e-08, 9.78709e-07)
G4WT5 > [2] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.85583e-08, 9.92978e-07)
G4WT5 > [3] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (7.5e-08, 1.0348e-06)
G4WT0 > [1] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (5.21242e-08, 1.011e-06)
G4WT1 > [2] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.86474e-09, 9.4848e-07)
G4WT5 > [4] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (8.19372e-09, 9.57983e-07)
G4WT5 > [5] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (3.80049e-08, 9.9346e-07)
G4WT7 > [3] Ionization of mu-α by cone 28 (x,y)=(6.75e-07, -1.5e-07) at (rho,z) = (3.78954e-08, 9.67811e-07)
G4WT6 > [2] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (7.5e-08, 1.03852e-06)
G4WT8 > --> Event 100 starts with initial seeds (85083220,78272982).
G4WT6 > [3] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.30722e-08, 9.79813e-07)
G4WT3 > [1] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.87298e-08, 1.01953e-06)
G4WT0 > [2] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.04636e-06)
G4WT8 > [1] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.01529e-06)
G4WT3 > [2] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (7.5e-08, 1.03039e-06)
G4WT8 > [2] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (5.04333e-08, 1.00271e-06)
G4WT1 > [3] Ionization of mu-α by cone 17 (x,y)=(7.5e-08, 0) at (rho,z) = (5.48296e-08, 9.84265e-07)
G4WT8 > [3] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (4.74974e-08, 1.03554e-06)
G4WT2 > WARNING: G4VisManager::EndOfEvent: Automatic event keeping suspended.
G4WT2 > 
  The number of events exceeds the maximum, 100, that may be kept by
  the vis manager.
G4WT3 > [3] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (1.10358e-08, 9.66119e-07)
G4WT3 > [4] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (5.406e-08, 1.02198e-06)
G4WT0 > [3] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (4.09588e-08, 1.01461e-06)
G4WT0 > [4] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (3.59108e-08, 9.91509e-07)
G4WT8 > [4] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.04026e-06)
G4WT4 > --> Event 200 starts with initial seeds (5306558,8004867).
G4WT8 > [5] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (7.5e-08, 1.03682e-06)
G4WT8 > [6] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (5.40065e-08, 1.04746e-06)
G4WT8 > [7] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (7.5e-08, 1.04027e-06)
G4WT5 > [6] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (4.30521e-08, 1.02464e-06)
G4WT0 > [5] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.65511e-08, 9.93498e-07)
G4WT2 > [1] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (4.54319e-08, 1.02829e-06)
G4WT8 > [8] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (1.39116e-08, 9.6197e-07)
G4WT6 > [4] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (7.5e-08, 1.01341e-06)
G4WT3 > [5] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.904e-08, 1.00842e-06)
G4WT3 > [6] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (6.19616e-08, 1.01061e-06)
G4WT8 > [9] Ionization of mu-α by cone 27 (x,y)=(5.25e-07, 1.5e-07) at (rho,z) = (1.58838e-08, 1.048e-06)
G4WT0 > [6] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.51234e-08, 1.01523e-06)
G4WT3 > --> Event 300 starts with initial seeds (47661162,10864742).
G4WT6 > [5] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (7.5e-08, 1.00781e-06)
G4WT5 > [7] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.96076e-08, 9.89901e-07)
G4WT1 > [4] Ionization of mu-α by cone 18 (x,y)=(7.5e-08, 1.5e-07) at (rho,z) = (6.19808e-08, 1.04242e-06)
G4WT6 > [6] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (5.38211e-08, 1.0281e-06)
G4WT0 > [7] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (7.5e-08, 1.01773e-06)
G4WT7 > [4] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.02942e-06)
G4WT4 > [1] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (3.39986e-08, 9.90677e-07)
G4WT5 > [8] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.00137e-06)
G4WT5 > [9] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.02482e-06)
G4WT5 > [10] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (4.33209e-08, 1.02933e-06)
G4WT5 > [11] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.01944e-06)
G4WT7 > [5] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (2.89533e-08, 9.75445e-07)
G4WT2 > [2] Ionization of mu-α by cone 26 (x,y)=(5.25e-07, 0) at (rho,z) = (4.53822e-08, 9.72171e-07)
G4WT2 > [3] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.04637e-06)
G4WT8 > [10] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.04011e-06)
G4WT8 > [11] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.01724e-06)
G4WT3 > [7] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (4.64598e-08, 1.02311e-06)
G4WT2 > [4] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.28183e-08, 9.94769e-07)
G4WT5 > --> Event 400 starts with initial seeds (42169047,2872570).
G4WT0 > [8] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (3.03483e-08, 9.81436e-07)
G4WT1 > [5] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.01002e-06)
G4WT5 > [12] Ionization of mu-α by cone 17 (x,y)=(7.5e-08, 0) at (rho,z) = (5.40642e-08, 9.98685e-07)
G4WT5 > [13] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (6.12863e-08, 1.01222e-06)
G4WT3 > [8] Ionization of mu-α by cone 17 (x,y)=(7.5e-08, 0) at (rho,z) = (5.2007e-08, 9.97467e-07)
G4WT3 > [9] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.77825e-08, 1.03219e-06)
G4WT4 > [2] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (7.5e-08, 1.0132e-06)
G4WT5 > [14] Ionization of mu-α by cone 16 (x,y)=(7.5e-08, -1.5e-07) at (rho,z) = (4.98874e-08, 1.048e-06)
G4WT5 > [15] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (2.87151e-08, 9.78101e-07)
G4WT6 > [7] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (3.79141e-08, 1.00081e-06)
G4WT8 > [12] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (1.53843e-08, 9.70534e-07)
G4WT3 > [10] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.37779e-08, 9.97092e-07)
G4WT3 > [11] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (4.21654e-08, 9.97951e-07)
G4WT3 > [12] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.02026e-06)
G4WT0 > [9] Ionization of mu-α by cone 20 (x,y)=(2.25e-07, 0) at (rho,z) = (1.84681e-08, 9.6011e-07)
G4WT0 > [10] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (6.30004e-08, 1.01119e-06)
G4WT0 > [11] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.03767e-06)
G4WT8 > --> Event 500 starts with initial seeds (58868730,8280117).
G4WT5 > [16] Ionization of mu-α by cone 26 (x,y)=(5.25e-07, 0) at (rho,z) = (5.08963e-08, 9.78395e-07)
G4WT3 > [13] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (3.53543e-08, 9.80165e-07)
G4WT0 > [12] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (7.5e-08, 1.02819e-06)
G4WT0 > [13] Ionization of mu-α by cone 6 (x,y)=(-5.25e-07, 1.5e-07) at (rho,z) = (3.24958e-08, 1.048e-06)
G4WT7 > [6] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 9.99761e-07)
G4WT4 > [3] Ionization of mu-α by cone 21 (x,y)=(2.25e-07, 1.5e-07) at (rho,z) = (3.47431e-08, 9.68082e-07)
G4WT3 > [14] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.03029e-06)
G4WT3 > --> Event 600 starts with initial seeds (56222359,3133739).
G4WT0 > [14] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 9.99133e-07)
G4WT8 > [13] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.01293e-06)
G4WT4 > [4] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (5.62382e-09, 9.64007e-07)
G4WT5 > [17] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (3.60581e-08, 9.86121e-07)
G4WT6 > [8] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (4.26865e-08, 9.92823e-07)
G4WT0 > [15] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.51961e-08, 1.00105e-06)
G4WT1 > [6] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (3.04056e-08, 9.79375e-07)
G4WT1 > [7] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (1.16508e-08, 9.6752e-07)
G4WT7 > [7] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (5.29167e-08, 1.03135e-06)
G4WT7 > --> Event 700 starts with initial seeds (33030306,21799208).
G4WT1 > [8] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.01965e-06)
G4WT1 > [9] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.6102e-08, 1.01619e-06)
G4WT1 > [10] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (5.15513e-08, 9.98478e-07)
G4WT8 > [14] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (5.52072e-08, 1.00731e-06)
G4WT8 > [15] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (4.82927e-08, 1.01262e-06)
G4WT8 > [16] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.04288e-06)
G4WT3 > [15] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.01154e-06)
G4WT3 > [16] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.98665e-08, 1.00995e-06)
G4WT6 > [9] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (4.31975e-08, 1.01318e-06)
G4WT3 > [17] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.63063e-08, 1.00196e-06)
G4WT0 > [16] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (7.5e-08, 1.04498e-06)
G4WT3 > [18] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (5.24364e-08, 1.04222e-06)
G4WT8 > [17] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (7.5e-08, 9.94188e-07)
G4WT7 > [8] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (5.16044e-08, 1.0283e-06)
G4WT7 > --> Event 800 starts with initial seeds (50335070,48302800).
G4WT6 > [10] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (5.03665e-08, 1.00539e-06)
G4WT4 > [5] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.45046e-08, 1.01008e-06)
G4WT1 > [11] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.79438e-08, 9.82305e-07)
G4WT5 > [18] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (5.89305e-08, 1.04434e-06)
G4WT0 > [17] Ionization of mu-α by cone 16 (x,y)=(7.5e-08, -1.5e-07) at (rho,z) = (5.05736e-08, 1.01538e-06)
G4WT0 > [18] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (4.973e-08, 1.02441e-06)
G4WT2 > [5] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.27662e-08, 1.04799e-06)
G4WT6 > [11] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.34221e-08, 9.85063e-07)
G4WT8 > [18] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.04026e-06)
G4WT4 > [6] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (2.90622e-08, 9.76358e-07)
G4WT1 > [12] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (3.96121e-08, 1.00225e-06)
G4WT4 > [7] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (4.89636e-08, 1.0147e-06)
G4WT4 > --> Event 900 starts with initial seeds (8431593,96717408).
G4WT5 > [19] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.01383e-06)
G4WT5 > [20] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (5.1304e-08, 1.03948e-06)
G4WT5 > [21] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.42064e-08, 9.82524e-07)
G4WT3 > [19] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (3.09602e-08, 9.82519e-07)
G4WT3 > [20] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (7.5e-08, 1.03316e-06)
G4WT2 > [6] Ionization of mu-α by cone 24 (x,y)=(3.75e-07, 1.5e-07) at (rho,z) = (4.81579e-08, 9.80607e-07)
G4WT2 > [7] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.63868e-08, 1.00083e-06)
G4WT7 > [9] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (4.91759e-08, 1.01573e-06)
G4WT7 > [10] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.03614e-06)
G4WT1 > [13] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (4.39491e-08, 9.97679e-07)
G4WT1 > [14] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.38722e-08, 9.97209e-07)
G4WT7 > [11] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (5.11635e-08, 1.03349e-06)
G4WT3 > [21] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (4.44836e-08, 9.86744e-07)
G4WT1 > [15] Ionization of mu-α by cone 28 (x,y)=(6.75e-07, -1.5e-07) at (rho,z) = (4.78093e-08, 9.87527e-07)
G4WT7 > [12] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (5.59823e-08, 1.03689e-06)
G4WT7 > [13] Ionization of mu-α by cone 2 (x,y)=(-6.75e-07, 0) at (rho,z) = (7.5e-08, 1.03672e-06)
G4WT8 > [19] Ionization of mu-α by cone 1 (x,y)=(-6.75e-07, -1.5e-07) at (rho,z) = (3.66349e-08, 9.86789e-07)
G4WT3 > [22] Ionization of mu-α by cone 13 (x,y)=(-7.5e-08, -1.5e-07) at (rho,z) = (7.5e-08, 1.02305e-06)
G4WT3 > [23] Ionization of mu-α by cone 3 (x,y)=(-6.75e-07, 1.5e-07) at (rho,z) = (3.28148e-08, 9.79908e-07)
G4WT3 > [24] Ionization of mu-α by cone 14 (x,y)=(-7.5e-08, 0) at (rho,z) = (7.5e-08, 1.03528e-06)
G4WT0 > [19] Ionization of mu-α by cone 15 (x,y)=(-7.5e-08, 1.5e-07) at (rho,z) = (4.76128e-08, 1.03866e-06)
"""

tsv_output = """
14	3.99119e-08	9.62914e-07
2	7.5e-08	1.00494e-06
28	3.78954e-08	9.67811e-07
14	7.5e-08	1.02942e-06
2	2.89533e-08	9.75445e-07
14	7.5e-08	9.99761e-07
15	5.29167e-08	1.03135e-06
13	5.16044e-08	1.0283e-06
1	4.91759e-08	1.01573e-06
14	7.5e-08	1.03614e-06
1	5.11635e-08	1.03349e-06
14	5.59823e-08	1.03689e-06
2	7.5e-08	1.03672e-06
13	7.5e-08	1.01529e-06
13	5.04333e-08	1.00271e-06
1	4.74974e-08	1.03554e-06
13	7.5e-08	1.04026e-06
1	7.5e-08	1.03682e-06
2	5.40065e-08	1.04746e-06
15	7.5e-08	1.04027e-06
1	1.39116e-08	9.6197e-07
27	1.58838e-08	1.048e-06
14	7.5e-08	1.04011e-06
14	7.5e-08	1.01724e-06
2	1.53843e-08	9.70534e-07
14	7.5e-08	1.01293e-06
2	5.52072e-08	1.00731e-06
3	4.82927e-08	1.01262e-06
14	7.5e-08	1.04288e-06
15	7.5e-08	9.94188e-07
14	7.5e-08	1.04026e-06
1	3.66349e-08	9.86789e-07
29	4.06346e-08	9.78709e-07
14	3.86474e-09	9.4848e-07
17	5.48296e-08	9.84265e-07
18	6.19808e-08	1.04242e-06
14	7.5e-08	1.01002e-06
3	3.04056e-08	9.79375e-07
3	1.16508e-08	9.6752e-07
13	7.5e-08	1.01965e-06
14	4.6102e-08	1.01619e-06
13	5.15513e-08	9.98478e-07
14	3.79438e-08	9.82305e-07
13	3.96121e-08	1.00225e-06
2	4.39491e-08	9.97679e-07
14	4.38722e-08	9.97209e-07
28	4.78093e-08	9.87527e-07
13	7.5e-08	1.04351e-06
15	7.5e-08	1.03852e-06
14	4.30722e-08	9.79813e-07
3	7.5e-08	1.01341e-06
2	7.5e-08	1.00781e-06
15	5.38211e-08	1.0281e-06
1	3.79141e-08	1.00081e-06
2	4.26865e-08	9.92823e-07
1	4.31975e-08	1.01318e-06
14	5.03665e-08	1.00539e-06
14	3.34221e-08	9.85063e-07
14	4.87298e-08	1.01953e-06
1	7.5e-08	1.03039e-06
1	1.10358e-08	9.66119e-07
2	5.406e-08	1.02198e-06
15	4.904e-08	1.00842e-06
13	6.19616e-08	1.01061e-06
1	4.64598e-08	1.02311e-06
17	5.2007e-08	9.97467e-07
13	4.77825e-08	1.03219e-06
13	4.37779e-08	9.97092e-07
1	4.21654e-08	9.97951e-07
14	7.5e-08	1.02026e-06
2	3.53543e-08	9.80165e-07
14	7.5e-08	1.03029e-06
14	7.5e-08	1.01154e-06
13	4.98665e-08	1.00995e-06
13	4.63063e-08	1.00196e-06
14	5.24364e-08	1.04222e-06
14	3.09602e-08	9.82519e-07
1	7.5e-08	1.03316e-06
14	4.44836e-08	9.86744e-07
13	7.5e-08	1.02305e-06
3	3.28148e-08	9.79908e-07
14	7.5e-08	1.03528e-06
1	5.21242e-08	1.011e-06
13	7.5e-08	1.04636e-06
2	4.09588e-08	1.01461e-06
2	3.59108e-08	9.91509e-07
15	4.65511e-08	9.93498e-07
13	4.51234e-08	1.01523e-06
15	7.5e-08	1.01773e-06
2	3.03483e-08	9.81436e-07
20	1.84681e-08	9.6011e-07
2	6.30004e-08	1.01119e-06
13	7.5e-08	1.03767e-06
2	7.5e-08	1.02819e-06
6	3.24958e-08	1.048e-06
14	7.5e-08	9.99133e-07
13	4.51961e-08	1.00105e-06
15	7.5e-08	1.04498e-06
16	5.05736e-08	1.01538e-06
3	4.973e-08	1.02441e-06
15	4.76128e-08	1.03866e-06
1	3.39986e-08	9.90677e-07
3	7.5e-08	1.0132e-06
21	3.47431e-08	9.68082e-07
2	5.62382e-09	9.64007e-07
14	4.45046e-08	1.01008e-06
2	2.90622e-08	9.76358e-07
13	4.89636e-08	1.0147e-06
2	4.54319e-08	1.02829e-06
26	4.53822e-08	9.72171e-07
13	7.5e-08	1.04637e-06
15	4.28183e-08	9.94769e-07
14	3.27662e-08	1.04799e-06
24	4.81579e-08	9.80607e-07
15	4.63868e-08	1.00083e-06
15	4.36511e-08	1.01882e-06
14	4.85583e-08	9.92978e-07
2	7.5e-08	1.0348e-06
3	8.19372e-09	9.57983e-07
3	3.80049e-08	9.9346e-07
1	4.30521e-08	1.02464e-06
14	3.96076e-08	9.89901e-07
13	7.5e-08	1.00137e-06
14	7.5e-08	1.02482e-06
3	4.33209e-08	1.02933e-06
13	7.5e-08	1.01944e-06
17	5.40642e-08	9.98685e-07
13	6.12863e-08	1.01222e-06
16	4.98874e-08	1.048e-06
1	2.87151e-08	9.78101e-07
26	5.08963e-08	9.78395e-07
3	3.60581e-08	9.86121e-07
14	5.89305e-08	1.04434e-06
14	7.5e-08	1.01383e-06
3	5.1304e-08	1.03948e-06
15	4.42064e-08	9.82524e-07
"""

# Function to parse (cone_id, rho, z) tuples from terminal lines
def parse_terminal(lines):
    pattern = re.compile(r"cone (\d+) .+? \(rho,z\) = \(\s*([^,]+),\s*([^)]+)\)")
    events = []
    for line in lines.splitlines():
        match = pattern.search(line)
        if match:
            cid = int(match.group(1))
            rho = float(match.group(2))
            z = float(match.group(3))
            events.append((cid, rho, z))
    return events

# Function to parse (cone_id, rho, z) tuples from TSV content
def parse_tsv(lines):
    events = []
    for line in lines.splitlines():
        parts = line.strip().split()
        if len(parts) == 3:
            cid, rho, z = parts
            try:
                events.append((int(cid), float(rho), float(z)))
            except ValueError:
                pass  # skip non-data lines
    return events

# Parse both lists
terminal_events = parse_terminal(terminal_output)
tsv_events = parse_tsv(tsv_output)

# Compare as multisets (order-independent)
from collections import Counter
terminal_counter = Counter(terminal_events)
tsv_counter = Counter(tsv_events)

# Find mismatches
only_in_terminal = terminal_counter - tsv_counter
only_in_tsv      = tsv_counter - terminal_counter

print("Events in terminal but not in TSV:")
for event, count in only_in_terminal.items():
    print(f"  {event} (count {count})")

print("\nEvents in TSV but not in terminal:")
for event, count in only_in_tsv.items():
    print(f"  {event} (count {count})")

# Summaries
print(f"\nTotal terminal events parsed: {sum(terminal_counter.values())}")
print(f"Total TSV events parsed: {sum(tsv_counter.values())}")