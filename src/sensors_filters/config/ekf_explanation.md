# Explicação da configuração do Extended Kalman Filter (EKF) .yaml

Dado que o modelo do robô utilizado é o **TurtleBot3** no simulador **Gazebo** num ambiente **planar 2D (XY)**, foi definido o parâmetro **two_d_mode: true**. Assim, o EKF ignora automaticamente todas as variáveis 3D (**Z**, **roll**, **pitch**, as suas respetivas **velocidades**, e **aceleração em Z**), restringindo a estimação a apenas: **x, y, yaw, v_x, v_y, v_yaw, a_x e a_y**

## Sensors Fusion
**Odometria (odom0):** Fundi as posições absolutas **(x,y)**, velocidades lineares **(v_x e v_y​)** e angulares **(v_yaw​)**. Apesar de ter usado **x** e **y**, as mesmas vão receber um menor peso na matriz de covariância do processo (Q) para que o EKF não connfie integralmente nestes dados, mitigando erros de predição causados por derrapagem das rodas (*drift* odométrico). Apesar do robo ser não-holonomico, ou seja, não consegue mover-se lateralmente instantaneamente, usei **v_y** como **true** para que o EKF tenha a informação de que a velocidade lateral é 0 (**restrição cinemática**). 

**IMU (imu0):** Fundi a aceleração **ax**, orientação **(yaw)** e a velocidade angular **(v_yaw​)**. Não usei **aceleração em y (ay)** pois o robo não tem aceleração em y idealmente. Contudo, o sensor de IMU pode medir valores diferentes de 0 (ruído) que pode fazer com que a estimação tenha drift.

## Covariance Matrices

**Matriz P (Initial Estimate Covariance):** Definida com 0.1 na diagonal principal para as variáveis ativas e 1e-9 (default) para as variáveis ignoradas. O valor 0.1 dita a incerteza inicial de quando o robô acaba de ser ligado, permitindo a convergência imediata para o estado real nos segundos iniciais de arranque e anulando erros de posição inicial. Para as variáveis 3D e acelerações inativas, utilizei 1e-9 (default) para fixar a incerteza em valores desprezáveis, garantindo que o filtro ignore qualquer ruído nestes eixos.

**Matriz Q (Process Noise Covariance):** Utilizei 0.1 na diagonal da matriz para as variáveis ativas (x,y,yaw e velocidades) para garantir uma convergência rápida no arranque do sistema pois quando o robô acaba de ser ligado, não temos a certeza absoluta da sua posição exata. Para as variáveis 3D e acelerações inativas, utilizei 1e-9 (valor default) para fixar a incerteza em valores desprezáveis, garantindo que o filtro ignore qualquer ruído nestes eixos e mantenha a estimativa estritamente planar

## Matrizes de Covariância
**Matriz P (Initial Estimate Covariance):** Definida com 0.1 na diagonal principal para as variáveis ativas e 1e-9 para as variáveis ignoradas. O valor 0.1 dita a incerteza inicial, permitindo a convergência imediata para o estado real nos segundos iniciais de arranque e anulando erros de posição inicial.

**Matriz Q (Process Noise Covariance):** Valores ajustados para 0.01 nos componentes de posição (x, y) e 0.05 nos restantes. A redução da incerteza do processo em **x** e **y** aumenta a inércia e a rigidez matemática do modelo cinemático. O filtro prioriza a fluidez da trajetória e rejeita saltos bruscos derivados de perdas momentâneas de tração.