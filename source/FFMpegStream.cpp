#include "FFMpegStream.h"



rtsp_unity_plugin::FFMpegStream::FFMpegStream(const char* uri)
{
	m_pStreamUri = uri;
}


rtsp_unity_plugin::FFMpegStream::~FFMpegStream()
{
}
int rtsp_unity_plugin::FFMpegStream::setMediaSink(MediaSink* media_sink)
{
	media_sink->setSrcCodecContext(m_pCodecCtx);
	m_pMediaSink = media_sink;
	return 0;
}

const char * rtsp_unity_plugin::FFMpegStream::getUri() const
{
	return m_pStreamUri;
}


/*
int rtsp_unity_plugin::FFMpegStream::addMediaSink(MediaSink &media_sink)
{
	media_sink.setSrcCodecContext(m_pCodecCtx);
	m_vMediaSink.push_back(media_sink);
	return 0;
}*/
/* will read frame and send them to the mediaSink
   if no frame are available, close the stream and return 0
   if frame not yet available, return 1
   else return 2; (2 mean ok!!)

*/
int rtsp_unity_plugin::FFMpegStream::ReadFrame() 
{
	if (m_hasInit && !m_isClosed) {
		if (av_read_frame(m_pFormatCtx, &m_packet) >= 0) {
			if (m_packet.stream_index == m_VideoStreamIndex) {
				//Supply raw packet data as input to a decoder.
				avcodec_send_packet(m_pCodecCtx, &m_packet);
				// Decode video frame
				m_frameFinished = avcodec_receive_frame(m_pCodecCtx, m_pFrameSrc);
				// Did we get a video complete video frame?
				if (m_frameFinished == 0) {
					/*
					for (std::vector<MediaSink>::iterator it = m_vMediaSink.begin(); it != m_vMediaSink.end(); ++it) {
						it->WriteVideo(m_pFrameSrc);
					}*/
					m_pMediaSink->WriteVideo(m_pFrameSrc);
				}
			}
			else if (m_packet.stream_index == m_AudioStreamIndex) {
				// TODO Handle the audio output!!!
				/*
				for (std::vector<MediaSink>::iterator it = m_vMediaSink.begin(); it != m_vMediaSink.end(); ++it) {
					it->WriteAudio(m_pFrameSrc);
				}*/
				m_pMediaSink->WriteAudio(m_pFrameSrc);
			}else {
				// if thie packet is an error, we free the packet
				av_packet_unref(&m_packet);
			}
		}
		else {
			CloseStream();
		}
	}
	else return 0;
}
