# Recrutamento ATLAS Software & AI

## Processamento de Dados dos Sensores

### Nós (C++)
* `lidar2d_filter.cpp`: Decimação e filtragem de limites para pointclouds 2D (LaserScan).
* `pcloud_downsample.cpp`: Redução de densidade (downsampling - VoxelGrid e PassThrough filter along x and z axis) para pointclouds 3D (PointCloud2).
* `imu_lowpass_filter.cpp`: Filtro passa-baixo (EMA - Exponential Moving Average) para estabilização de medições inerciais.

### Launch Files
* `lidar2d.launch.py`: Inicializa `lidar2d_filter.cpp` e `imu_lowpass_filter.cpp` (TurtleBot3, Gazebo Classic v.11).
* `pcloud3d.launch.py`: Inicializa `pcloud_downsample.cpp` e `imu_lowpass_filter.cpp` (TurtleBot4, Gazebo Fortress).
* `ekf.launch.py`: Inicializa nós de filtragem 2D/IMU integrados com `ekf_node` do pacote `robot_localization` (TurtleBot3, Gazebo Classic v.11).
* `slam2d.launch.py`: Integra pipeline sensorial completo (filtros, EKF) com o nó `sync_slam_toolbox_node` do pacote `slam_toolbox` (TurtleBot3, Gazebo Classic v.11).

### Registo de Dados (Bags)
* `slam2d_bag`: Registo sensorial de teste para avaliação comparativa de parâmetros do algoritmo SLAM (Default vs. Tunado).

### Artefactos de Mapeamento (Maps)
* **Gerados via SLAM Toolbox (2D):**
    * `mapa_default`: Mapeamento síncrono via *rosbag* utilizando a configuração default.
    * `mapa_tunado`: Mapeamento síncrono via *rosbag* utilizando a configuração tunada.
    * `mapa_completo`: Mapeamento em tempo real do ambiente simulado integral via teleoperação manual.
* **Gerados via RTAB-Map (3D):**
    * `slam3d`:

### Resultados de Estrutura Cinemática
Árvores de transformações extraídas via `ros2 run tf2_tools view_frames`:
* `frames_default.pdf`: Hierarquia nativa do simulador.
* `frames_ekf.pdf`: Hierarquia validada com sobreposição do nó `robot_localization`.
* `frames_ekf_slam.pdf`: Hierarquia contínua estendendo `map` $\rightarrow$ `odom` $\rightarrow$ `base_footprint`.

### Configurações de Interface (RViz2)
* `scanfilter.rviz`
* `pcloud3_filter.rviz`
* `ekf.rviz`
* `slam2d.rviz`

### Parâmetros (Config)
* `ekf_config.yaml`: Matrizes de covariância e configuração de fusão sensorial.
* `slam_toolbox_config.yaml`: Limitações de alcance LiDAR e densificação do grafo de poses.

---

## Execução e Comparação do SLAM Toolbox

A sequência de comandos requer execução a partir da raiz do *workspace* (ex: `~/atlas_recruitment_ws`).

```bash
# 0. Extração de dados (sem ativação de SLAM)
ros2 launch sensors_filters ekf.launch.py
ros2 bag record /scan_filtered /odometry/filtered /tf /tf_static

# (Terminar o processo ekf.launch.py após gravação do trajeto)

# 1. Inicialização Síncrona do SLAM Toolbox (Escolher 1.1 OU 1.2)

# 1.1 Configuração Default
ros2 run slam_toolbox sync_slam_toolbox_node --ros-args -p use_sim_time:=true -p odom_frame:=odom -p base_frame:=base_footprint -p map_frame:=map -r /scan:=/scan_filtered

# 1.2 Configuração Tunada
ros2 run slam_toolbox sync_slam_toolbox_node --ros-args --params-file ./src/sensors_filters/config/slam_toolbox_config.yaml -p use_sim_time:=true -r /scan:=/scan_filtered

# 2. Monitorização Visual
ros2 run rviz2 rviz2 -d ./src/sensors_filters/rviz/slam2d.rviz 

# 3. Injeção de Dados Sincronizados
ros2 bag play ./src/sensors_filters/bags/slam2d_bag/slam2d_bag.db3 --clock

# 4. Exportação do mapa final
ros2 run nav2_map_server map_saver_cli -f mapa_x 