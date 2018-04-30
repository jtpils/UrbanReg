#include "registration_pipeline.h"

#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <DataIO/include/data_io.h>
#include <GPSLocalizer/include/gps.h>
#include <OverlapTrimmer/include/trimmer.h>

REGPIPELINE_PUBLIC void RegPipeline::LocalizePointCloud(const std::vector<std::string> &filenames, std::string &format)
{
  if (filenames.size() < 4) return;
  
  Eigen::MatrixXd A;
  if (format == "OBJ" || format == "obj")
  {
    Eigen::MatrixXi F;
    igl::readOBJ(filenames[0], A, F);
  }
  if (format == "PLY" || format == "ply")
  {
    Eigen::MatrixXd C;
    DataIO::read_ply(filenames[0], A, C);
  }

  Eigen::MatrixXd L, G;
  Eigen::MatrixXi F;
  igl::readOBJ(filenames[1], L, F);
  igl::readOBJ(filenames[2], G, F);

  double s;
  Eigen::Matrix3d R;
  Eigen::RowVector3d T;
  GPS::CalcualteCamerasTransformation(L, G, s, R, T);
  Eigen::MatrixXd B = GPS::GetLocalizedPoints(A, s, R, T);

  if (format == "OBJ" || format == "obj")
    igl::writeOBJ(filenames[3], B, Eigen::MatrixXi());
  if (format == "PLY" || format == "ply")
    DataIO::write_ply(filenames[3], B, Eigen::MatrixXd());
}

REGPIPELINE_PUBLIC void RegPipeline::TrimPointsClouds(const std::vector<std::string> &filenames,
  std::string &in_format)
{
  if (filenames.size() < 4) return;

  Eigen::MatrixXd A, B;
  if (in_format == "OBJ" || in_format == "obj")
  {
    Eigen::MatrixXi F;
    igl::readOBJ(filenames[0], A, F);
    igl::readOBJ(filenames[1], B, F);
  }
  if (in_format == "PLY" || in_format == "ply")
  {
    Eigen::MatrixXd C;
    DataIO::read_ply(filenames[0], A, C);
    DataIO::read_ply(filenames[1], B, C);
  }

  Trimmer::Trim(A, B, 0.05);

  igl::writeOBJ(filenames[2], A, Eigen::MatrixXi());
  igl::writeOBJ(filenames[3], B, Eigen::MatrixXi());
}
