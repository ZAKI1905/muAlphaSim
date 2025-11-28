# from dirac_hydrogenic import DiracHydrogenicSolver

import dirac_hydrogenic as dh

# Parameters
# Z = 2  # Charge of Helium nucleus
# m_mu_MeV = 105.6583745
# m_He_MeV = 3727.3794066
# m_mu = m_mu_MeV * 1e6 * dh.const.e / dh.c**2
# m_He = m_He_MeV * 1e6 * dh.const.e / dh.c**2
# m_r = m_mu * m_He / (m_mu + m_He)

from dirac_bound_state_solver import DiracBoundStateSolver
import scipy.constants as const

# Hydrogen parameters
Z = 1
m_e_MeV = 0.51099895000
m_p_MeV = 938.27208816

m_e = m_e_MeV * 1e6 * const.e / const.c**2
m_p = m_p_MeV * 1e6 * const.e / const.c**2
m_r = m_e * m_p / (m_e + m_p)

# Create solver
solver = DiracBoundStateSolver(Z=Z, m_r=m_r)

# Solve 1s
E1s_H, r_H, G_H, F_H = solver.solve_state(n=1, kappa=-1)
print(f"Hydrogen 1s binding energy: {1e3*E1s_H:.10f} eV")

# Plot
import matplotlib.pyplot as plt
plt.figure(figsize=(10,6))
plt.plot(r_H*1e15, G_H, label='G 1s')
plt.plot(r_H*1e15, F_H, label='F 1s')
plt.xlabel('r [fm]')
plt.ylabel('Radial functions (normalized)')
plt.legend()
plt.grid(True)
plt.show()

# # Example: 5p3/2
# E5p, r5, G5, F5 = solver.solve_state(n=5, kappa=-2)
# print(f"5p binding energy: {E5p:.10f} keV")

# # Plot if desired
# import matplotlib.pyplot as plt
# plt.figure(figsize=(10,6))
# plt.plot(r1*1e15, G1, label='G 1s')
# plt.plot(r1*1e15, F1, label='F 1s')
# plt.plot(r5*1e15, G5, label='G 5p')
# plt.plot(r5*1e15, F5, label='F 5p')
# plt.xlabel('r [fm]')
# plt.ylabel('Radial functions (normalized)')
# plt.legend()
# plt.grid(True)
# plt.show()