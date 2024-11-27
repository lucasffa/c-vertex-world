# Voxel Viewer

Este é um projeto em C que apresenta um mundo de voxels renderizados em formato de cubos, permitindo visualizar e navegar pelo mundo usando as setas do teclado.

## Estrutura do Projeto

- `src/`: Contém os arquivos fonte em C.
- `include/`: Contém arquivos de cabeçalho adicionais.
- `assets/`:
  - `textures/`: Contém as texturas usadas para os blocos.
    - `grass.png`
    - `dirt.png`
    - `stone.png`
  - `shaders/`: Contém os shaders usados na renderização.
    - `vertex_shader.glsl`
    - `fragment_shader.glsl`
- `build/`: Diretório onde os arquivos objeto serão compilados.
- `bin/`: Diretório onde o executável será gerado.
- `Makefile`: Arquivo para compilar o projeto.
- `README.md`: Este arquivo.

## Compilação e Execução

### Dependências

- GCC
- OpenGL
- GLFW
- GLEW
- cglm (biblioteca para matemática vetorial)
- stb_image.h (incluído no projeto para carregamento de imagens)

Certifique-se de que as bibliotecas acima estejam instaladas em seu sistema.

### Compilação

Execute o comando:

```bash
make
```

O executável será gerado no diretório `bin/` com o nome `voxel_viewer`.

### Execução

Execute o programa com:

```bash
./bin/voxel_viewer
```

## Controles

- **Setas do teclado**: Movimenta a câmera pelo mundo.

## Estrutura do Mundo

O mundo é dividido em chunks de 32x64x32 blocos (X x Y x Z), com 9 chunks (3x3) totalizando uma área maior. Os blocos disponíveis são grama, terra e pedra.

## Observações

Este projeto é um exemplo básico e pode ser expandido com funcionalidades adicionais, como geração procedural de terreno, otimizações de renderização, iluminação, entre outros.
```# c-vertex-world
