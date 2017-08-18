#include "../include/MyShader.h"

MyShader::MyShader(const GLchar *vert_path, const GLchar *frag_path)
{
	//以下这段代码用于将文件中的着色器源代码转换成流
	//再将流转换成C-字符串作为glShaderSource()的参数
	std::string vertex_shader_str;     //用于存储文件流的string对象
	std::string fragment_shader_str;
	std::ifstream vert_file;           //文件流
	std::ifstream frag_file;

	//打开着色器源代码文件，如果文件流中有badbit，则抛出异常
	vert_file.exceptions(std::ifstream::badbit);
	frag_file.exceptions(std::ifstream::badbit);
	try
	{
		vert_file.open(vert_path);
		frag_file.open(frag_path);
		//读取文件流到string对象
		std::stringstream vert_stream, frag_stream;
		vert_stream << vert_file.rdbuf();
		frag_stream << frag_file.rdbuf();
		vert_file.close();
		frag_file.close();
		vertex_shader_str = vert_stream.str();
		fragment_shader_str = frag_stream.str();
	}
	catch (std::ifstream::failure erro)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	//将string对象转换为C-字符串类型的着色器源代码
	const GLchar *vert_shader_source = vertex_shader_str.c_str();
	const GLchar *frag_shader_source = fragment_shader_str.c_str();

	//构建着色器对象
	GLuint vertex_shader;
	GLint success;
	GLchar info_log[512];
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	//链接着色器对象到shader_program，供Use()调用
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}
	//链接到program之后就可以把着色器对象删除掉以释放内存了
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}