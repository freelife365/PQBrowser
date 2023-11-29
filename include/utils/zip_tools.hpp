#pragma once

#include <regex>
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;

#include <libzippp/libzippp.h>
using namespace libzippp;

#include "logger.hpp"

namespace MBase
{

	class ZipTools
	{
	public:
		ZipTools(const std::string& docxFile)
		{
			m_zip = std::make_unique<ZipArchive>(docxFile);
			if (m_zip) {
				m_zip->open(ZipArchive::ReadOnly);
			}
		}

		~ZipTools()
		{
			if (m_zip) {
				m_zip->close();
			}
		}

		ZipArchive* GetZip()
		{
			if (m_zip) {
				return m_zip.get();
			}
			return nullptr;
		}

		std::string ReadXmlFromDocxAsText(const std::string& xmlName)
		{
			std::string strData("");
			if (m_zip) {
				for (auto& v : m_zip->getEntries()) {
					std::string name = v.getName();
					if (xmlName == name) {
						strData = v.readAsText();
						break;
					}
				}
			}
			return strData;
		}

		bool ExtractMediaFiles(const std::string& saveDir, std::vector<std::string>& vMediaFiles)
		{
			if (!m_zip) {
				return false;
			}

			std::vector<ZipEntry> entries = m_zip->getEntries();
			if (entries.size() <= 0) {
				LOGGER_ERROR("WordMediaParser", "Entries size: {}", entries.size());
				return false;
			}

			std::regex rgxWmfFile(R"(word/media/image[0-9]+.[wmf,emf])");
			std::regex rgxJpgFile(R"(word/media/image[0-9]+.[jpg,jpeg])");
			std::regex rgxPngFile(R"(word/media/image[0-9]+.png)");

			for (auto it = entries.begin(); it != entries.end(); ++it)
			{
				ZipEntry entry = *it;
				std::string name = entry.getName();

				if (std::regex_search(name, rgxWmfFile)
					|| std::regex_search(name, rgxJpgFile)
					|| std::regex_search(name, rgxPngFile)) {
					std::uint64_t size = entry.getSize();
					void* binaryData = entry.readAsBinary();
					if (binaryData) {
						fs::path p(entry.getName());
						std::string file_name = p.filename().string();
						std::string name = p.stem().string();
						std::string extension = p.extension().string();
						std::string local_path = fmt::format("{}{}", saveDir, file_name);

						try {
							//输出文件到本地
							std::ofstream file(local_path, std::ofstream::binary);
							file.write((char*)binaryData, size);
							file.close();
							delete[] binaryData;

							vMediaFiles.emplace_back(local_path);
						}
						catch (const std::exception& e) {
							LOGGER_ERROR("WordMediaParser", "Error:{}, Extract Media to {} Failed！", e.what(), local_path);
						}
					}
				}
				else {
					//TODO 其他图片类型
				}
			}

			return true;
		}


		static void TraverseAllFile(fs::path root_path, ZipArchive* zip)
		{
			if (fs::exists(root_path))
			{
				for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ root_path })
				{
					fs::path p(dir_entry);
					auto pos = root_path.string().size();
					auto entry = p.string().substr(pos);

					if (fs::is_directory(dir_entry)) {
						zip->addEntry(entry);
					}
					else {
						zip->addFile(entry, p.string());
					}
				}
			}
		}

		static bool CreateZipFile(const std::string& zipName, const std::string& appendDir)
		{
			ZipArchive zf(zipName);
			zf.open(ZipArchive::New);

			//遍历appendDir目录
			TraverseAllFile(appendDir, &zf);
			zf.close();

			auto count = zf.getEntries().size();
			return count > 0;
		}

	private:
		std::unique_ptr<ZipArchive> m_zip;
	};
}