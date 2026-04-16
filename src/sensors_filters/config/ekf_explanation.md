# Explicação da configuração do Extended Kalman Filter .yaml

Dado que o modelo do robo utilizado é o **TurtleBot3** no simulador **Gazebo** num ambiente **planar 2D (XY)**, foi definido o parametro **two_d_mode: true**. Assim, o EKF ignora automaticamente todas as variáveis 3D, como variações em **Z**, **roll**, **pitch**, as suas respetivas **velocidades**, e **aceleração em Z**. Assim, restringindo a estimação para apenas: **x, y, yaw, v_x, v_y, v_yaw, a_x e a_y**

## Sensors Fusion
**Odometria (odom0):** Fundi as posições absolutas **(x,y)**, velocidades lineares **(v_x e v_y​)** e angulares **(v_yaw​)**. Apesar de ter usado **x** e **y** estas vão ter um menor peso na matrix de Covariancia para que o EKF não connfie tanto nestes dados, dado que as rodas podem derrapar (drift) o que faz com que estrague a predição do filtro. Apesar do robo ser não-holonomico, ou seja, não consegue mover-se lateralmente instantaneamente, usei **v_y** como **true** para que o EKF tenha a informação de que a velocidade lateral é 0 (**restrição cinemática**). 

**IMU (imu0):** Fundi a orientação **(yaw)** e a velocidade angular **(v_yaw​)**. Não usei **aceleração em y** pois o robo não tem aceleração em y idealmente. Contudo, o sensor de IMU pode medir valores diferentes de 0 como ruído que pode fazer com que a estimação tenha drift.

## Covariance Matrix
**Matriz P (Initial Estimate Covariance):** Definida com 0.1 na diagonal principal para as variáveis estimadas (x e y) e 1e-9 (valor default) para as variáveis de estado ignoradas. Defini como 0.1 **x** e **y** de modo a que Este valor é superior a Q para permitir que o filtro tenha "liberdade" para convergir rapidamente para o estado real logo nos primeiros segundos após o arranque, corrigindo qualquer erro na posição inicial.

**Matriz Q (Process Noise Covariance):** Utilizei 0.1 na diagonal da matriz para as variáveis ativas (x,y,yaw e velocidades) para garantir uma convergência rápida no arranque do sistema pois quando o robô acaba de ser ligado, não temos a certeza absoluta da sua posição exata. Para as variáveis 3D e acelerações inativas, utilizei 1e-9 (valor default) para fixar a incerteza em valores desprezáveis, garantindo que o filtro ignore qualquer ruído nestes eixos e mantenha a estimativa estritamente planar