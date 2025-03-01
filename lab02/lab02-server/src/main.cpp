#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "restbed"
#include "test.hpp"
#include <memory>
#include <string>
#include <unordered_map>

static int maxProductId = -1;

class Product {
  public:
	Product(const char *name, const char *desc)
		: name(std::string(name)), desc(std::string(desc)), icon(""),
		  iconData({}) {
	}
	Product(const Product &) = default;
	std::string name, desc, icon;
	std::vector<char> iconData;
};

static std::unordered_map<int, Product> products = {};

void productPostHandler(const std::shared_ptr<restbed::Session> session) {
	const auto request = session->get_request();

	size_t contentLength = request->get_header("Content-Length", 0);

	session->fetch(
		contentLength,
		[request](const std::shared_ptr<restbed::Session> session,
				  const restbed::Bytes &body) {
			rapidjson::Document d;
			d.Parse((const char *)body.data(), body.size());

			assert(d.HasMember("name"));
			assert(d["name"].IsString());
			assert(d.HasMember("description"));
			assert(d["description"].IsString());

			products.insert(
				{++maxProductId,
				 Product(d["name"].GetString(), d["description"].GetString())});

			d.AddMember("id", maxProductId, d.GetAllocator());

			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			d.Accept(writer);

			session->close(
				restbed::OK, buffer.GetString(),
				{{"Content-Length", std::to_string(buffer.GetLength())},
				 {"Connection", "close"}});
		});
}

void productIdPutHandler(const std::shared_ptr<restbed::Session> session) {
	const auto request = session->get_request();
	size_t contentLength = request->get_header("Content-Length", 0);

	int id = std::stoi(request->get_path_parameter("productId"));

	session->fetch(
		contentLength,
		[request, id](const std::shared_ptr<restbed::Session> session,
					  const restbed::Bytes &body) {
			if (products.contains(id)) {
				rapidjson::Document d;
				d.Parse((const char *)body.data(), body.size());

				if (d.HasMember("name") && d["name"].IsString()) {
					products.at(id).name = std::string(d["name"].GetString());
				}
				if (d.HasMember("description") && d["description"].IsString()) {
					products.at(id).desc =
						std::string(d["description"].GetString());
				}

				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

				writer.StartObject();
				writer.Key("id");
				writer.Int(id);
				writer.Key("name");
				writer.String(products.at(id).name);
				writer.Key("description");
				writer.String(products.at(id).desc);
				writer.EndObject();

				session->close(
					restbed::OK, buffer.GetString(),
					{{"Content-Length", std::to_string(buffer.GetLength())},
					 {"Connection", "close"}});
			} else {
				session->close(restbed::NOT_FOUND);
			}
		});
}

void productIdGetHandler(const std::shared_ptr<restbed::Session> session) {
	const auto request = session->get_request();
	size_t contentLength = request->get_header("Content-Length", 0);

	int id = std::stoi(request->get_path_parameter("productId"));

	session->fetch(
		contentLength,
		[request, id](const std::shared_ptr<restbed::Session> session,
					  const restbed::Bytes &) {
			if (products.contains(id)) {
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

				writer.StartObject();
				writer.Key("id");
				writer.Int(id);
				writer.Key("name");
				writer.String(products.at(id).name);
				writer.Key("description");
				writer.String(products.at(id).desc);
				writer.EndObject();

				session->close(
					restbed::OK, buffer.GetString(),
					{{"Content-Length", std::to_string(buffer.GetLength())},
					 {"Connection", "close"}});
			} else {
				session->close(restbed::NOT_FOUND);
			}
		});
}

void productIdDeleteHandler(const std::shared_ptr<restbed::Session> session) {
	const auto request = session->get_request();
	size_t contentLength = request->get_header("Content-Length", 0);

	int id = std::stoi(request->get_path_parameter("productId"));

	session->fetch(
		contentLength,
		[request, id](const std::shared_ptr<restbed::Session> session,
					  const restbed::Bytes &) {
			if (products.contains(id)) {
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

				writer.StartObject();
				writer.Key("id");
				writer.Int(id);
				writer.Key("name");
				writer.String(products.at(id).name);
				writer.Key("description");
				writer.String(products.at(id).desc);
				writer.EndObject();
				products.erase(id);

				session->close(
					restbed::OK, buffer.GetString(),
					{{"Content-Length", std::to_string(buffer.GetLength())},
					 {"Connection", "close"}});
			} else {
				session->close(restbed::NOT_FOUND);
			}
		});
}

void productsGetHandler(const std::shared_ptr<restbed::Session> session) {
	const auto request = session->get_request();
	size_t contentLength = request->get_header("Content-Length", 0);

	session->fetch(
		contentLength,
		[request](const std::shared_ptr<restbed::Session> session,
				  const restbed::Bytes &) {
			rapidjson::Document d;
			d.SetArray();
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			for (auto &p : products) {
				rapidjson::Value v;
				v.SetObject();
				v.AddMember("id", p.first, d.GetAllocator());
				v.AddMember("name", p.second.name, d.GetAllocator());
				v.AddMember("description", p.second.desc, d.GetAllocator());
				d.PushBack(v, d.GetAllocator());
			}
			d.Accept(writer);
			session->close(
				restbed::OK, buffer.GetString(),
				{{"Content-Length", std::to_string(buffer.GetLength())},
				 {"Connection", "close"}});
		});
}
int main(const int, const char **) {
	auto resource = std::make_shared<restbed::Resource>();
	resource->set_path("/product");
	resource->set_method_handler("POST", productPostHandler);

	auto resource3 = std::make_shared<restbed::Resource>();
	resource3->set_path("/products");
	resource3->set_method_handler("GET", productsGetHandler);

	auto resource2 = std::make_shared<restbed::Resource>();
	resource2->set_path("/product/{productId: [0-9][0-9]*}");
	resource2->set_method_handler("GET", productIdGetHandler);
	resource2->set_method_handler("PUT", productIdPutHandler);
	resource2->set_method_handler("DELETE", productIdDeleteHandler);

	restbed::Service service;
	auto settings = std::make_shared<restbed::Settings>();
	settings->set_port(8080);
	service.publish(resource);
	service.publish(resource2);
	service.publish(resource3);
	service.start(settings);

	printInt(5);

	return EXIT_SUCCESS;
}
