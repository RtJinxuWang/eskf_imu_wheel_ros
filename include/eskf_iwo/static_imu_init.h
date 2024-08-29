//
// Created by jinxu on 2024/07/30.
//

#ifndef SLAM_IN_AUTO_DRIVING_STATIC_IMU_INIT_H
#define SLAM_IN_AUTO_DRIVING_STATIC_IMU_INIT_H

// #include "../common/eigen_types.h"
// #include "../common/imu.h"
// #include "../common/odom.h"
#include <eskf_iwo/common/eigen_types.h>
#include <eskf_iwo/common/imu.h>
#include <eskf_iwo/common/odom.h>

#include <deque>

namespace sad 
{
    /**
     * IMU 水平静止状态下初始化器
     * 使用方法：调用 AddIMU, AddOdom 添加数据，使用 InitSuccess 获取初始化是否成功
     * 成功后，使用各 Get 函数获取内部参数
     *
     * 初始化器在每次调用 AddIMU 时尝试对系统进行初始化。在有 odom 的场合，初始化要求 odom 轮速读数接近零；没有时，假设车辆初期静止。
     * 初始化器收集一段时间内的 IMU 读数，按照书本 3.5.4 节估计初始零偏和噪声参数，提供给 ESKF 或者其他滤波器
     */
    class StaticIMUInit 
    {
        public:
            // 静态初始化
            struct Options 
            {
                Options() {}
                double init_time_seconds_ = 10.0;            // 静止时间
                int init_imu_queue_max_size_ = 2000;         // 初始化IMU队列最大长度
                int static_odom_pulse_ = 5;                  // 静止时轮速计输出噪声
                double max_static_gyro_var = 0.5;            // 静态下陀螺测量方差
                double max_static_acce_var = 0.05;           // 静态下加计测量方差
                double gravity_norm_ = 9.81;                 // 重力大小
                bool use_speed_for_static_checking_ = true;  // 是否使用odom来判断车辆静止（部分数据集没有odom选项）
            };

            // 构造函数
            StaticIMUInit(Options options = Options()) : options_(options) {}

            // 添加IMU数据
            bool AddIMU(const IMU& imu);
            // 添加轮速数据
            bool AddOdom(const Odom& odom);

            // 判定初始化是否成功
            bool InitSuccess() const { return init_success_; }

            // 获取各 Cov, bias, gravity
            Vec3d GetCovGyro() const { return cov_gyro_; }
            Vec3d GetCovAcce() const { return cov_acce_; }
            Vec3d GetInitBg() const { return init_bg_; }
            Vec3d GetInitBa() const { return init_ba_; }
            Vec3d GetGravity() const { return gravity_; }

        private:
            // 尝试对系统初始化
            bool TryInit();

            Options options_;                 // 选项信息
            bool init_success_ = false;       // 初始化是否成功
            Vec3d cov_gyro_ = Vec3d::Zero();  // 陀螺测量噪声协方差（初始化时评估）
            Vec3d cov_acce_ = Vec3d::Zero();  // 加计测量噪声协方差（初始化时评估）
            Vec3d init_bg_ = Vec3d::Zero();   // 陀螺初始零偏
            Vec3d init_ba_ = Vec3d::Zero();   // 加计初始零偏
            Vec3d gravity_ = Vec3d::Zero();   // 重力
            bool is_static_ = false;          // 标志车辆是否静止
            std::deque<IMU> init_imu_deque_;  // 初始化用的数据
            double current_time_ = 0.0;       // 当前时间
            double init_start_time_ = 0.0;    // 静止的初始时间
    };

}  // namespace sad

#endif  // SLAM_IN_AUTO_DRIVING_STATIC_IMU_INIT_H