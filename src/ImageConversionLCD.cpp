// -*- C++ -*-
/*!
 * @file  ImageConversionLCD.cpp
 * @brief Image Conversion EV3 LCD
 * @date $Date$
 *
 * @author 宮本　信彦　n-miyamoto@aist.go.jp
 * 産業技術総合研究所　ロボットイノベーション研究センター
 * ロボットソフトウエアプラットフォーム研究チーム
 *
 * LGPL
 *
 * $Id$
 */

#define DEFAULT_WIDTH 192
#define DEFAULT_HEIGHT 128


#include "ImageConversionLCD.h"



// Module specification
// <rtc-template block="module_spec">
static const char* imageconversionlcd_spec[] =
  {
    "implementation_id", "ImageConversionLCD",
    "type_name",         "ImageConversionLCD",
    "description",       "Image Conversion EV3 LCD",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "ImageProcessing",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageConversionLCD::ImageConversionLCD(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inIn("in", m_in),
    m_outOut("out", m_out)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ImageConversionLCD::~ImageConversionLCD()
{
}



RTC::ReturnCode_t ImageConversionLCD::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("in", m_inIn);
  
  // Set OutPort buffer
  addOutPort("out", m_outOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onFinalize()
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onActivated(RTC::UniqueId ec_id)
{
	m_out.width = DEFAULT_WIDTH;
	m_out.height = DEFAULT_HEIGHT;
	
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onExecute(RTC::UniqueId ec_id)
{
	if (m_inIn.isNew())
	{
		m_inIn.read();

		cv::Mat color_img;
		color_img.create(cv::Size(m_in.width, m_in.height), CV_8UC3);
		memcpy(color_img.data, (void *)&(m_in.pixels[0]), m_in.pixels.length());


		cv::Mat resize_img;
		cv::resize(color_img, resize_img, cv::Size(m_out.width, m_out.height));

		cv::Mat gray_img;
		cv::cvtColor(resize_img, gray_img, CV_RGB2GRAY);

		cv::Mat result_image;
		cv::threshold(gray_img, result_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

		int num = m_out.width * m_out.height;
		
		unsigned char *temp_data = new unsigned char[num];
		for (unsigned int i = 0; i < m_out.height; i++)
		{
			for (unsigned int j = 0; j < m_out.width; j++)
			{
				//std::cout << i*w + j + (data_w - w)*i << "\t" << i << "\t" << j << std::endl;
				temp_data[i*m_out.width + j] = result_image.data[i*m_out.width + j];
			}
		}

		m_out.pixels.length(num/8);
		for (int i = 0; i < num / 8; i++)
		{
			m_out.pixels[i] = 0x00;
			for (int j = 0; j < 8; j++)
			{
				if (temp_data[i * 8 + j] == 0x00)
				{
					m_out.pixels[i] += 0x01 << j;

				}
			}
		}

		delete temp_data;


		
		setTimestamp(m_out);
		m_outOut.write();
	}
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageConversionLCD::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}



extern "C"
{
 
  void ImageConversionLCDInit(RTC::Manager* manager)
  {
    coil::Properties profile(imageconversionlcd_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageConversionLCD>,
                             RTC::Delete<ImageConversionLCD>);
  }
  
};


