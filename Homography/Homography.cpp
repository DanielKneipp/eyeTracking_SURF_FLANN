/*
 * Referencias:
 * 1 - http://www.ic.unicamp.br/~rocha/teaching/2012s1/mc949/aulas/additional-material-revision-of-concepts-homography-and-related-topics.pdf
 * 2 - http://docs.opencv.org/doc/tutorials/features2d/feature_homography/feature_homography.html
 * 3 - http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
 * 4 - http://felix.abecassis.me/?s=skew+angle
 * 5 - http://answers.opencv.org/question/14807/fill-an-image-with-the-content-of-rotatedrect/
 * 6 - http://answers.opencv.org/question/497/extract-a-rotatedrect-area/
 * 7 - http://stackoverflow.com/questions/24046089/calculating-skew-of-text-opencv
 */

#include "Homography.h"

eyeT::Homography::Homography() : obj_corners (std::vector <cv::Point2f> (4)),
                                 scene_obj_corners (std::vector <cv::Point2f> (4)),
                                 transformed_perpective (false), rotated_rect_isCurrent(false),
                                 rect_isCurrent (false)
{}

void eyeT::Homography::find_homography (std::vector <cv::KeyPoint>& obj_keyPoints,
                                        std::vector <cv::KeyPoint>& scene_keyPoints,
                                        std::vector <cv::DMatch>& good_matches)
{
    for (cv::DMatch& good_match : good_matches)
    {
        //-- Get the keypoints from the good matches[2]
        this->obj.push_back (obj_keyPoints[good_match.queryIdx].pt);
        this->scene.push_back (scene_keyPoints[good_match.trainIdx].pt);
    }

    this->H = cv::findHomography (obj, scene, CV_RANSAC);
    this->transformed_perpective = false;
    this->rotated_rect_isCurrent = false;
}

void eyeT::Homography::perspective_transform (cv::Size& obj_img_size)
{
    this->obj_corners[0] = cv::Point (0,0);
    this->obj_corners[1] = cv::Point (obj_img_size.width,0);
    this->obj_corners[2] = cv::Point (obj_img_size.width, obj_img_size.height);
    this->obj_corners[3] = cv::Point (0,obj_img_size.height);

    cv::perspectiveTransform( this->obj_corners, this->scene_obj_corners, this->H );
    this->transformed_perpective = true;
}

bool eyeT::Homography::draw_lines (cv::Mat& img,cv::Point2f& offset)
{
	if (!this->transformed_perpective)
	    return false;
		
    cv::line (img, this->scene_obj_corners[0] + offset, this->scene_obj_corners[1] + offset, cv::Scalar(0,255,0), 4);
    cv::line (img, this->scene_obj_corners[1] + offset, this->scene_obj_corners[2] + offset, cv::Scalar(0,255,0), 4);
    cv::line (img, this->scene_obj_corners[2] + offset, this->scene_obj_corners[3] + offset, cv::Scalar(0,255,0), 4);
    cv::line (img, this->scene_obj_corners[3] + offset, this->scene_obj_corners[4] + offset, cv::Scalar(0,255,0), 4);
    return true;
}

cv::RotatedRect eyeT::Homography::get_rotated_rect()
{
    if (!this->transformed_perpective)
        return cv::RotatedRect();

    this->rotated_rect = cv::minAreaRect( cv::Mat( this->scene_obj_corners ) );
    this->rotated_rect_isCurrent = true;
    return this->rotated_rect;
}

cv::Mat eyeT::Homography::get_rotated_rect_img (cv::Mat &img)
{
    if( !this->rotated_rect_isCurrent )
        return false;

    /// Testar o uso de '.angle' como angulo de rotacao
    cv::Mat rot_mat = cv::getRotationMatrix2D( this->rotated_rect.center, this->rotated_rect.angle, 1);
    cv::warpAffine( img, this->rotated_rect_img, img.size, cv::INTER_CUBIC);

    return this->rotated_rect_img;
}