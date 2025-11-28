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
print(f"Hydrogen 1s binding energy: {E1s_H:.10f} eV")

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